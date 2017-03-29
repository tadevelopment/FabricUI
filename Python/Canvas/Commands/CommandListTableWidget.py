#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import re, json, os
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions, Commands
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.HotkeyEditor.CommandAction import CommandAction
from FabricEngine.Canvas.HotkeyEditor.CommandListTableWidgetActionItem import CommandListTableWidgetActionItem
from FabricEngine.Canvas.HotkeyEditor.CommandListTableWidgetItemDelegate import CommandListTableWidgetItemDelegate

class CommandListTableWidget(QtGui.QTableWidget):

    """ CommandListTableWidget is used to associate a shortcut to an action/command.
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

    def __init__(self, parent, canvasWindow):
        """ Initializes the CommandListTableWidget.
            
            Arguments:
            - parent: A reference to wiget.
            - canvasWindow: A reference the canvasWindow.
        """
        super(CommandListTableWidget, self).__init__(parent)

        
        # Used to attached the Command actions
        self.canvasWindow = canvasWindow

        # Lists of changed items
        # Use as a temp buffer so actions' 
        # shorcut are not override as soon 
        # as the user assign a new kwy.
        self.__changedActionDic = {}

        # Notify when an action is registered-
        # unregistered from Actions.ActionRegistry.
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        actionRegistry.actionRegistered.connect(self.__onActionRegistered)
        actionRegistry.actionUnregistered.connect(self.__onActionUnregistered)

        # Notify when an command is registered.
        GetCommandRegistry().commandRegisteredCallback.connect(self.__onCommandRegistered)
 
        # Construct the item-delegate
        itemDelegate = CommandListTableWidgetItemDelegate()
        itemDelegate.keyPressed.connect(self.__onSetItemKeySequence)
        self.setItemDelegate(itemDelegate)

        # Two coloums: [actionName, shortcut]
        self.setColumnCount(1)
        self.setHorizontalHeaderItem(0, QtGui.QTableWidgetItem('Command'))
        #self.setHorizontalHeaderItem(1, QtGui.QTableWidgetItem('Shortcut'))
        
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragEnabled(False)
        self.setTabKeyNavigation(False)
        self.setSortingEnabled(True)
        self.verticalHeader().setVisible(False)
        self.horizontalHeader().setStretchLastSection(True)

        # qss
        self.setObjectName('CommandListTableWidget')
  
    def __getCurrentShortcutItem(self):
        """ \internal.
            Gets the current shortcut item
        """
        if self.currentColumn() == self.columnCount()-1:
            return self.item(self.currentRow(), self.currentColumn())

        return None

    def __getShorcutItem(self, actionName):
        """ \internal.
            Gets the shortcut item from the command name.
        """
        items = self.findItems(actionName, QtCore.Qt.MatchExactly)
        if items:
            return self.item(items[0].row(), 1)
        return None

    def __getActionItem(self, actionName):
        """ \internal.
            Gets the shortcut item from the command name.
        """
        items = self.findItems(actionName, QtCore.Qt.MatchExactly)
        if items:
            return self.item(items[0].row(), 0)
        return None

    def __getShortcutFromJSON(self, actionName):
        """ \internal.
            Returns the shortcut in the json data
            from the command name.
        """
        if self.jsonData and actionName in self.jsonData:
            shortcutList = self.jsonData[actionName]
            if shortcutList:
                return shortcutList[0]
        return None

    def __createNewRow(self, cmd):
        """ \internal.
            Create a new row: [actionName, shortcut] items.
        """

        tooltip = cmd.getHelp()
        isScriptable = issubclass(type(cmd), Commands.BaseScriptableCommand)

        
        rowCount = self.rowCount() 
        self.insertRow(rowCount)
        
        # 1. Action item
        item = CommandListTableWidgetActionItem(actionName, action.toolTip()) 
        self.setItem(rowCount, 0, item)

        # 2. Shortcut item
        # Get the shorcut, from the action or the json data.
        shortcut = self.__getShortcutFromJSON(actionName)
        if shortcut:
            actionRegistry = Actions.ActionRegistry.GetActionRegistry()
            actionRegistry.setShortcut(actionName, QtGui.QKeySequence(shortcut))
        else:
            shortcut = action.shortcut().toString(QtGui.QKeySequence.NativeText)

        # Check if we can edit the action is editable
        isEditable = True
        if issubclass(type(action), Actions.BaseAction):
            isEditable = action.isEditable()

        item = QtGui.QTableWidgetItem(shortcut)
        if not isEditable:
            item.setFlags(QtCore.Qt.NoItemFlags)
          
        self.setItem(rowCount, 1, item)

        # Hack to refresh the view correctly.
        self.setVisible(False)
        self.resizeColumnsToContents()
        self.setVisible(True)

    def __onCommandRegistered(self, cmdName, cmdType, implType):
        """ \internal.
            Called when an command has been registered in CommandRegistry.
            Create an action associated to the command. 
        """
    
        # Must construct the command to get the tooltip
        cmd = GetCommandRegistry().createCommand(cmdName)
        
        # Add the action to the canvasWindow so it's available.
        # Actions of hidden widgets are not triggered.
        self.__createNewRow(cmdName, cmd)
  