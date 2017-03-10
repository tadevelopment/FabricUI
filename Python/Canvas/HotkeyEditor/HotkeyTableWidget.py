#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import re, json, os
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions, Commands
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.HotkeyEditor.CommandAction import CommandAction
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableWidgetActionItem import HotkeyTableWidgetActionItem
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableWidgetItemDelegate import HotkeyTableWidgetItemDelegate

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

    def __init__(self, parent, canvasWindow):
        """ Initializes the HotkeyTableWidget.
            
            Arguments:
            - parent: A reference to wiget.
            - canvasWindow: A reference the canvasWindow.
        """
        super(HotkeyTableWidget, self).__init__(parent)

        # shortcuts saving.
        self.filename = None
        self.jsonData = None

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
        itemDelegate = HotkeyTableWidgetItemDelegate()
        itemDelegate.keyPressed.connect(self.__onSetItemKeySequence)
        self.setItemDelegate(itemDelegate)

        # Two coloums: [actionName, shortcut]
        self.setColumnCount(2)
        self.setHorizontalHeaderItem(0, QtGui.QTableWidgetItem('Action'))
        self.setHorizontalHeaderItem(1, QtGui.QTableWidgetItem('Shortcut'))
        
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragEnabled(False)
        self.setTabKeyNavigation(False)
        self.setSortingEnabled(True)
        self.verticalHeader().setVisible(False)
        self.horizontalHeader().setStretchLastSection(True)

        # qss
        self.setObjectName('HotkeyTableWidget')
  
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

    def mousePressEvent(self, event):
        """ Implementation of QtGui.QTableWidget.
            Directly edit the item to simulate a double-click.
        """
        super(HotkeyTableWidget, self).mousePressEvent(event)
        item = self.__getCurrentShortcutItem()
        if item is not None:
            self.editItem(item)

    def mouseDoubleClickEvent(self, event):
        """ Implementation of QtGui.QTableWidget.
            Do nothing.
        """
        pass

    def __createNewRow(self, actionName, action):
        """ \internal.
            Create a new row: [actionName, shortcut] items.
        """
        
        rowCount = self.rowCount() 
        self.insertRow(rowCount)
        
        # 1. Action item
        item = HotkeyTableWidgetActionItem(actionName, action.toolTip()) 
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
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        if actionRegistry.getAction(cmdName) is None:
            # Must construct the command to get the tooltip
            cmd = GetCommandRegistry().createCommand(cmdName)
            
            tooltip = cmd.getHelp()
            isScriptable = issubclass(type(cmd), Commands.BaseScriptableCommand)

            # Add the action to the canvasWindow so it's available.
            # Actions of hidden widgets are not triggered.
            self.canvasWindow.addAction(
                CommandAction(
                    self, cmdName, QtGui.QKeySequence(), tooltip, isScriptable))
 
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
        hotkeyTableWidgetActionItem = self.__getActionItem(actionName)
        if hotkeyTableWidgetActionItem:
            action.changed.connect(hotkeyTableWidgetActionItem.onActionChanged)

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

    def __canShortcutBeUsed(self, actionName, shortcut):
        """ \internal.
            Check if the command named `actionName` can use the shorcut.
            If true, returns None. If none, returns the name of the 
            command using the short cut alredy.

            Arguments:
            - actionName: The action that wants to use the shorcut.
            - shortcut: The shortcut to use.
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        otherWithShortcut = actionRegistry.isShortcutUsed(shortcut)
        otherWithShortcutName = actionRegistry.getActionName(otherWithShortcut)
 
        if otherWithShortcut and otherWithShortcutName not in self.__changedActionDic:
            action = actionRegistry.getAction(actionName) 

            # Check if the current action or the one having the shortcut already is global.
            isActionGlobal = (action.shortcutContext() == QtCore.Qt.WindowShortcut or  
                             action.shortcutContext() == QtCore.Qt.ApplicationShortcut) 

            isOtherWithShortcutGlobal = (otherWithShortcut.shortcutContext() == QtCore.Qt.WindowShortcut or  
                             otherWithShortcut.shortcutContext() == QtCore.Qt.ApplicationShortcut)

            # If both actions are local (to their widget),
            # check if they have the same parent.
            sharedSameParent = False
            if not isActionGlobal and not isOtherWithShortcutGlobal:
                sharedSameParent = otherWithShortcut.parentWidget() == action.parentWidget()
 
            return otherWithShortcutName
          
        elif not otherWithShortcut and otherWithShortcutName in self.__changedActionDic:
            return otherWithShortcutName

        else:
            # Lists of changed items
            for actionName_, shortcut_ in self.__changedActionDic.iteritems():
                if shortcut_ == shortcut:
                    return actionName_

        return None

    def __onSetItemKeySequence(self, keySequence):
        """ \internal.
            Sets the keySequence to the current 
            item when the user press a key.
        """
        item = self.__getCurrentShortcutItem()
        if item is not None:
            self.__setIemsKeySequence(item, keySequence)

    def __setIemsKeySequence(self, item, keySequence, force = False):
        """ \internal.
            Sets the keySequence to the item. If another items/actions 
            used the keySequence already, a warning is displayed and 
            the method returns. if 'force' is true, force to setting.

            If a keySequence is invalid or corresponds the `Del` or 
            `Backspace` key, the item keySequence is invalidate.
        """
        actionName = self.item(item.row(), 0).text()
        
        if  (  keySequence != QtGui.QKeySequence() and 
                keySequence != QtGui.QKeySequence('Del') and 
                keySequence != QtGui.QKeySequence('Backspace') ):

            shortcut = keySequence.toString(QtGui.QKeySequence.NativeText)
            actionName_ = self.__canShortcutBeUsed(actionName, keySequence)
                
            if actionName_ and not force:
                message = QtGui.QMessageBox()
                message.warning(self, 'Hotkey editor', 'shorcut ' + str(shortcut) + 
                    ' already used by ' + str(actionName_))

            elif not actionName_ or force:
                item.setText(shortcut)
                self.__changedActionDic[actionName] = keySequence

        else:
            item.setText('')
            self.__changedActionDic[actionName] = QtGui.QKeySequence()

    def filterItems(self, query, show = 'All'):
        """ \internal.
            Filters the items according the actions' names or shorcuts.
            To filter by shortcut, use '#' before the query.  
        """
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

            if  (   (searchByShortcut and regex.search(shortCut.lower()) ) or 
                    (not searchByShortcut and regex.search(actionName.lower()) ) ):

                isCommand = GetCommandRegistry().isCommandRegistered(actionName)
                
                if show == 'All':
                    self.setRowHidden(i, False)
                
                elif show == 'Actions':
                    if isCommand:
                        self.setRowHidden(i, True)
                    else:
                        self.setRowHidden(i, False)

                elif show == 'Commands':
                    if isCommand:
                        self.setRowHidden(i, False)
                    else:
                        self.setRowHidden(i, True)

            # If nothing found, hide the row.    
            else:
                self.setRowHidden(i, True)

    def acceptShortcutChanges(self):
        """ \internal.
            Accepts the changes. Sets the actions shortcuts 
            from the items text.
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        for actionName, keySequence in self.__changedActionDic.iteritems():
            # Set the same shortcut to all the  
            # actions registered as `actionName`.
            actionRegistry.setShortcut(actionName, keySequence)

        self.__changedActionDic = {}

    def rejectShortcutChanges(self):
        """ \internal.
            Rejects the changes. Sets the items text from
            the actions shortcuts. 
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        for actionName in self.__changedActionDic:
            keySequence = actionRegistry.getShortcut(actionName)
            item = self.__getShorcutItem(actionName)
            if item:
                item.setText(keySequence.toString(QtGui.QKeySequence.NativeText))

        self.__changedActionDic = {} 

    def openActions(self):
        """ \internal.
            Opens a json file containing a list of pair {actionName, shortcut}
            that override the current actions' shortcuts (if matched). Report 
            a warning if changes have niot been saved.
        """
        if len(self.__changedActionDic) > 0:
            message = QtGui.QMessageBox()
            message.warning(self, 'Hotkey editor', 'Changes not saved')
            return False

        lastDir = str(self.canvasWindow.settings.value("hotkeyEditor/lastFolder"))
        fname, _ = QtGui.QFileDialog.getOpenFileName(self, "Open Hotkey file", lastDir, "*.json")

        fname = str(fname)
        if len(fname) == 0:
            return False

        else:
            # Sets the current file
            self.filename = fname

            # Override the shortcuts 
            # from the json matches.
            self.jsonData = None
            with open(self.filename) as infile:  
                self.jsonData = json.load(infile)

            for actionName, shortcutList in self.jsonData.iteritems():
                item = self.__getShorcutItem(actionName)
                if item:  
                    shortcut = ''    
                    if shortcutList:
                        shortcut = shortcutList[0]
                        
                    if shortcut != item.text():
                        self.__setIemsKeySequence(item, QtGui.QKeySequence(shortcut), True)

            self.acceptShortcutChanges()
            return True

    def saveActions(self):
        """ \internal.
            Saves in the current json file a list of pair 
            {actionName, shortcut} of the current actions. 
        """
        # If no filename, call save as.
        if not self.filename and not self.saveActionsAs():
            return

        self.acceptShortcutChanges()

        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        self.jsonData = {}
        for actionName in actionRegistry.getActionNameList():
            keySequenceList = actionRegistry.getShortcuts(actionName)
            
            keyStringSequenceList = []
            for keySequence in keySequenceList:
                keyStringSequenceList.append(keySequence.toString(QtGui.QKeySequence.NativeText))
            
            self.jsonData[actionName] = keyStringSequenceList
         
        with open(self.filename, 'w') as outfile:  
            json.dump(self.jsonData, outfile, ensure_ascii=False, indent=4)

    def saveActionsAs(self):
        """ \internal.
            Saves in a new json file a list of pair 
            {actionName, shortcut} of the current actions. 
        """
        fname = self.filename
        
        if not fname:
            fname = str(self.canvasWindow.settings.value("hotkeyEditor/lastFolder"))
        fname, _ = QtGui.QFileDialog.getSaveFileName(self, "Save Hotkey file", fname, "*.json")
        
        if not fname:
            return False

        self.canvasWindow.settings.setValue("hotkeyEditor/lastFolder", os.path.dirname(fname))
        self.filename = fname

        self.saveActions()
  