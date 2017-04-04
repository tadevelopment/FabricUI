#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import re
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions, Commands
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.HotkeyEditor.CommandAction import CommandAction
from FabricEngine.Canvas.HotkeyEditor.HotKeyTableWidgetItems import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableWidgetItemDelegate import HotkeyTableWidgetItemDelegate
from FabricEngine.Canvas.HotkeyEditor.SetActionItemKeySequenceCommand import SetActionItemKeySequenceCommand
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableManager import HotkeyTableManager


class HotkeyTableWidget(QtGui.QTableWidget):

    """ HotkeyTableWidget is used to associate a shortcut to an action/command.
        When an action or command is registered, a signal is sent to the 
        table so it creates a new item. Items can be filtered according 
        to the actions name or shortcut. 

        To associate a new shortcut to an action, the user needs first to 
        select which action item to update and then press a key. When done, 
        the user can accept or reject the changes to actually update the 
        actions' shortcuts.
         
        A list of pair {actionName, shortcut} can be saved in as json.
        The file can be opened and sets the shortucts of the actions it 
        lists. When an action is registered, we check if it's litsed in
        the file to override its shortcut.
    """

    editingItem = QtCore.Signal(bool)
 
    def __init__(self, parent, canvasWindow):
        """ Initializes the HotkeyTableWidget.
            
            Arguments:
            - parent: A reference to wiget.
            - canvasWindow: A reference the canvasWindow.
        """
        super(HotkeyTableWidget, self).__init__(parent)

        self.qUndoStack = QtGui.QUndoStack()

        # Used to attached the Command actions
        self.canvasWindow = canvasWindow

        self.manager = HotkeyTableManager(
            self,
            self.canvasWindow.settings
            )

        self.manager.updateShortcutItem.connect(
            self.__onUpdateShortcutItem
            )

        # Notify when an action is registered-
        # unregistered from Actions.ActionRegistry.
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        actionRegistry.actionRegistered.connect(self.__onActionRegistered)
        actionRegistry.actionUnregistered.connect(self.__onActionUnregistered)

        # Notify when an command is registered.
        GetCommandRegistry().commandRegisteredCallback.connect(self.__onCommandRegistered)
        
        # Construct the item-delegate
        itemDelegate = HotkeyTableWidgetItemDelegate(self)
        itemDelegate.keyPressed.connect(self.__onSetItemKeySequence)
        self.setItemDelegate(itemDelegate)

        # Two coloums: [actionName, shortcut]
        self.setColumnCount(2)
        self.setHorizontalHeaderItem(0, QtGui.QTableWidgetItem('Name'))
        self.setHorizontalHeaderItem(1, QtGui.QTableWidgetItem('Shortcut'))
 
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragEnabled(False)
        self.setSortingEnabled(True)
        self.verticalHeader().setVisible(False)
        self.horizontalHeader().setStretchLastSection(True)

        # qss
        self.setObjectName('HotkeyTableWidget')

    def onEmitEditingItem(self, status):
        self.editingItem.emit(status)

    def __onUpdateShortcutItem(self, actionName, shortcut):
        item = self.__getShorcutItem(actionName)
        if item :
            item.setText(shortcut)
            self.scrollToItem(item)

    def __getCurrentShortcutItem(self):
        """ \internal.
            Gets the current shortcut item
        """
        if self.currentColumn() == self.columnCount()-1:
            return self.item(self.currentRow(), self.currentColumn())

    def __getShorcutItem(self, actionName):
        """ \internal.
            Gets the shortcut item from the command name.
        """
        items = self.findItems(actionName, QtCore.Qt.MatchExactly)
        if items:
            return self.item(items[0].row(), 1)

    def __getActionItem(self, actionName):
        """ \internal.
            Gets the shortcut item from the command name.
        """
        items = self.findItems(actionName, QtCore.Qt.MatchExactly)
        if items:
            return self.item(items[0].row(), 0)
 
    def mousePressEvent(self, event):
        """ Implementation of QtGui.QTableWidget.
            Directly edit the item to simulate a double-click.
        """

        self.setCurrentItem(None)

        super(HotkeyTableWidget, self).mousePressEvent(event)
        
        item = self.__getCurrentShortcutItem()
        if item is not None:
            self.onEmitEditingItem(True)
            self.editItem(item)
        else:
            self.onEmitEditingItem(False)

    def mouseDoubleClickEvent(self, event):
        """ Implementation of QtGui.QTableWidget.
            Do nothing.
        """
        pass

    def keyboardSearch(self, search):
        """ Implementation of QtGui.QAbstractItemView.
            Do nothing.
        """
        pass

    def __createNewRow(self, actionName, action):
        """ \internal.
            Create a new row: [actionName, shortcut] items.
        """
        
        rowCount = self.rowCount() 
        self.insertRow(rowCount)
        
        # Check if the action is editable
        isEditable = True
        if issubclass(type(action), Actions.BaseAction):
            isEditable = action.isEditable()

        # Check if the action is associated to a command.
        isCommand = None
        if issubclass(type(action), CommandAction):
            isCommand = action.implType
            
        # 1. Action item
        item = ActionTableWidgetItem(
            actionName, 
            action.toolTip(), 
            isEditable,
            isCommand
            ) 

        self.setItem(rowCount, 0, item)

        # 2. Shortcut item
        # Get the shorcut, from the action or the json data.
        shortcut = self.manager.getShortcutFromJSON(actionName)
        if shortcut:
            actionRegistry = Actions.ActionRegistry.GetActionRegistry()
            actionRegistry.setShortcut(actionName, QtGui.QKeySequence(shortcut))
        else:
            shortcut = action.shortcut().toString(QtGui.QKeySequence.NativeText)

        isActionGlobal = self.manager.isActionContextGlobal(action) 

        item = ShorcutTableWidgetItem(
            shortcut, 
            isEditable, 
            isActionGlobal
            ) 

        self.setItem(rowCount, 1, item)

        self.resizeColumnsToContents()

    def __onCommandRegistered(self, cmdName, cmdType, implType):
        """ \internal.
            Called when an command has been registered in CommandRegistry.
            Create an action associated to the command. 
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        if actionRegistry.getAction(cmdName) is None:
            # Must construct the command to get the tooltip
            cmd = GetCommandRegistry().createCommand(cmdName)
            
            tooltip = cmdType+ "[" + implType + "]\n\n"
            tooltip += cmd.getHelp()
            isScriptable = issubclass(type(cmd), Commands.BaseScriptableCommand)

            # Add the action to the canvasWindow so it's available.
            # Actions of hidden widgets are not triggered.
            action = CommandAction(
                self, 
                cmdName, 
                implType,
                QtGui.QKeySequence(), 
                tooltip, 
                isScriptable
                )

            self.canvasWindow.addAction(action)
 
    def __onActionRegistered(self, actionName, action):
        """ \internal.
            Called when an action has been registered in Actions.ActionRegistry.
            Create an item associated to the action. 
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        # Check it's the first time the action is registered.
        if actionRegistry.getRegistrationCount(actionName) == 1:     
            self.__createNewRow(actionName, action)
        
        # To update the item tool tip.
        actionTableWidgetItem = self.__getActionItem(actionName)
        if actionTableWidgetItem:
            action.changed.connect(actionTableWidgetItem.onActionChanged)

    def __onActionUnregistered(self, actionName):
        """ \internal.
            Called when an action has been  
            unregistered from Actions.ActionRegistry.
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        
        # Check there is no more action registered under `actionName`.
        if actionRegistry.getRegistrationCount(actionName) == 0:
            # if so, remove the item.
            item = self.__getShorcutItem(actionName)
            if item:
                self.removeRow(item.row())

    def __onSetItemKeySequence(self, keySequence):
        """ \internal.
            Sets the keySequence of the current 
            item when the user press a key.
        """
        item = self.__getCurrentShortcutItem()
        actionName = self.item(item.row(), 0).text()

        currentKeySequence = QtGui.QKeySequence(item.text())

        if item and keySequence != currentKeySequence:

            cmd = SetActionItemKeySequenceCommand( 
                self.qUndoStack,
                self.manager, 
                actionName, 
                currentKeySequence,
                keySequence
                ) 

            if cmd.doIt():
                self.qUndoStack.push(cmd)

    def filterItems(self, query, edit = 0, show = 0):
        """ \internal.
            Filters the items according the actions' names or shorcuts.
            To filter by shortcut, use '#' before the query.  
        """

        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        searchByShortcut = False
        if len(query):
            searchByShortcut = query[0] == '#'

        # Remove white-space and non alpha-num
        pattern = query.lower().replace(' ', '')
        regex = re.compile('[^0-9a-zA-Z]')
        pattern = regex.sub('', pattern)

        # Converts 'abc' to 'a.*b.*c'
        pattern = '.*'.join(pattern)   
        regex = re.compile(pattern) 

        for i in range(0, self.rowCount()):

            # Checks the action's name/shortcut matches.
            actionName = self.item(i, 0).text()
            shortCut = self.item(i, 1).text()

            isEditable = True
            action = actionRegistry.getAction(actionName)
            if issubclass(type(action), Actions.BaseAction):
                isEditable = action.isEditable()
 
            if  (   (searchByShortcut and regex.search(shortCut.lower()) ) or 
                    (not searchByShortcut and regex.search(actionName.lower()) ) ):

                isCommand = GetCommandRegistry().isCommandRegistered(actionName)
                
                showEditable = True
                if edit == 1 and isEditable == False:
                    showEditable = False
                elif edit == 2 and isEditable == True:
                    showEditable = False

                hide = not showEditable

                # Show actions + cmds
                if show == 0:
                    self.setRowHidden(i, hide)
                
                # Show actions  
                elif show == 1:
                    if isCommand:
                        self.setRowHidden(i, True)
                    else:
                        self.setRowHidden(i, hide)

                # Show cmd  
                elif show == 2:
                    if isCommand:
                        self.setRowHidden(i, hide)
                    else:
                        self.setRowHidden(i, True)

            # If nothing found, hide the row.    
            else:
                self.setRowHidden(i, True)
