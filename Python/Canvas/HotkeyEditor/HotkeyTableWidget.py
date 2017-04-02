#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import re, json, os
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions, Commands
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.HotkeyEditor.CommandAction import CommandAction
from FabricEngine.Canvas.HotkeyEditor.HotKeyTableWidgetItems import *
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

    stateIsDirty = QtCore.Signal(bool, bool)
    editingItem = QtCore.Signal(bool)
 
    def __init__(self, parent, canvasWindow):
        """ Initializes the HotkeyTableWidget.
            
            Arguments:
            - parent: A reference to wiget.
            - canvasWindow: A reference the canvasWindow.
        """
        super(HotkeyTableWidget, self).__init__(parent)

        self.isDirty = False 
        self.isFileDirty = False 

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

    def __getCurrentShortcutItem(self):
        """ \internal.
            Gets the current shortcut item
        """
        print "self.currentColumn() " + str(self.currentColumn())
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
        self.setCurrentItem(None)
        super(HotkeyTableWidget, self).mousePressEvent(event)
        
        item = self.__getCurrentShortcutItem()
        if item is not None:
            self.editingItem.emit(True)
            self.editItem(item)
        else:
            self.editingItem.emit(False)

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

        # 1. Action item
        tooltip = action.toolTip()
        item = ActionTableWidgetItem(actionName, action.toolTip(), isEditable) 
        self.setItem(rowCount, 0, item)

        # 2. Shortcut item
        # Get the shorcut, from the action or the json data.
        shortcut = self.__getShortcutFromJSON(actionName)
        if shortcut:
            actionRegistry = Actions.ActionRegistry.GetActionRegistry()
            actionRegistry.setShortcut(actionName, QtGui.QKeySequence(shortcut))
        else:
            shortcut = action.shortcut().toString(QtGui.QKeySequence.NativeText)

        isActionGlobal = self.__isActionContextGlobal(action) 
        item = ShorcutTableWidgetItem(shortcut, isEditable, isActionGlobal)          
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
            
            tooltip = "Command, " + cmdType+ "[" + implType + "]\n\n"
            tooltip += cmd.getHelp()
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

    def __isActionContextGlobal(self, action):
        """ \internal.
            Check if the action' context is global:
            WindowShortcut or ApplicationShortcut. 
        """
        return (action.shortcutContext() == QtCore.Qt.WindowShortcut or  
                action.shortcutContext() == QtCore.Qt.ApplicationShortcut) 

    def __isShortcutBeUsed(self, actionName, shortcut):
        """ \internal.
            Check if the command named `actionName` can use the shorcut.
            If true, returns None. If none, returns the name of the 
            command using the short cut alredy.

            Arguments:
            - actionName: The action that wants to use the shorcut.
            - shortcut: The shortcut to use.
        """
 
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        otherActionList = actionRegistry.isShortcutUsed(shortcut)

        for otherActionName, shortcut_ in self.__changedActionDic.iteritems():
            if shortcut_ == shortcut:
                otherActionList.append(actionRegistry.getAction(otherActionName))
        
        if actionName in otherActionList:
            otherActionName.remve(actionName)

        action = actionRegistry.getAction(actionName) 
        isActionContextGlobal = self.__isActionContextGlobal(action) 

        for otherAction in otherActionList:
            otherActionName = actionRegistry.getActionName(otherAction)
       
            isOtherActionContextGlobal = self.__isActionContextGlobal(otherAction) 
 
            # If both actions are local (to their widget),
            # check if they have the same parent.
            sharedSameParent = False
            if not isOtherActionContextGlobal and not isActionContextGlobal:
                sharedSameParent = otherAction.parentWidget() == action.parentWidget()
  
            if isOtherActionContextGlobal or isActionContextGlobal or sharedSameParent:
                return otherActionName
 
        return None

    def __onSetItemKeySequence(self, keySequence):
        """ \internal.
            Sets the keySequence to the current 
            item when the user press a key.
        """
        item = self.__getCurrentShortcutItem()
        if item is not None:
            self.__setItemKeySequence(item, keySequence)
            return True
        return False

    def __setStateIsDirty(self, dirty):
        self.isDirty = dirty
        self.stateIsDirty.emit(self.isDirty, self.isFileDirty)

    def __setFileStateIsDirty(self, dirty):
        self.isFileDirty = dirty
        self.stateIsDirty.emit(self.isDirty, self.isFileDirty)

    def __setItemKeySequenceAndShortcut(self, item, actionName, keySequence = None, shortcut = None):
        """ \internal.
        """
        if shortcut:
            item.setText(shortcut)
            self.__changedActionDic[actionName] = keySequence
        
        else:
            item.setText('')
            self.__changedActionDic[actionName] = QtGui.QKeySequence()

        self.__setStateIsDirty(True)

    def __setItemKeySequence(self, item, keySequence, force = False):
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
            actionName_ = self.__isShortcutBeUsed(actionName, keySequence)
                
            if actionName_ and not force:
                message = QtGui.QMessageBox()
                message.warning(self, 'Hotkey editor', 'shorcut ' + str(shortcut) + 
                    ' already used by ' + str(actionName_))

            elif not actionName_ or force:
                self.__setItemKeySequenceAndShortcut(item, actionName, keySequence, shortcut)
        
        else:
            self.__setItemKeySequenceAndShortcut(item, actionName)
        
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

    def __resetChangedActionDic(self):
        """ \internal.
        """
        self.__changedActionDic = {}
        self.__setStateIsDirty(False)

    def acceptShortcutChanges(self, fileStateIsDirty = True):
        """ \internal.
            Accepts the changes. Sets the actions shortcuts 
            from the items text.
        """

        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        for actionName, keySequence in self.__changedActionDic.iteritems():
            # Set the same shortcut to all the  
            # actions registered as `actionName`.
            actionRegistry.setShortcut(actionName, keySequence)

        self.__resetChangedActionDic()
        self.__setFileStateIsDirty(fileStateIsDirty)

    def rejectShortcutChanges(self):
        """ \internal.
            Rejects the changes. Sets the items text from
            the actions shortcuts. 
        """

        hasChanged = len(self.__changedActionDic) > 0

        if hasChanged:
            actionRegistry = Actions.ActionRegistry.GetActionRegistry()

            for actionName in self.__changedActionDic:
                keySequence = actionRegistry.getShortcut(actionName)
                item = self.__getShorcutItem(actionName)
                if item:
                    item.setText(keySequence.toString(QtGui.QKeySequence.NativeText))

            self.__resetChangedActionDic()
 
    def __checkIfSaveNeeded(self):
        """ \internal.
            Check if the changes needs to be saved.
        """

        if len(self.__changedActionDic) > 0:
            msgBox = QtGui.QMessageBox(
                QtGui.QMessageBox.Warning, 
                'Hotkey editor', 
                'Changes not saved',
                QtGui.QMessageBox.Yes | QtGui.QMessageBox.No | QtGui.QMessageBox.Cancel,
                self
                );
            msgBox.setInformativeText("Do you want to save your changes?")
            
            msgBox.setDefaultButton(QtGui.QMessageBox.Yes)
            ret = msgBox.exec_()

            if ret == QtGui.QMessageBox.Yes:
                self.saveActions()

            elif ret == QtGui.QMessageBox.Cancel:
                return True

        return False

    def openActions(self):
        """ \internal.
            Opens a json file containing a list of pair {actionName, shortcut}
            that override the current actions' shortcuts (if matched). Report 
            a warning if changes have niot been saved.
        """
        if self.__checkIfSaveNeeded():
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
                        self.__setItemKeySequence(item, QtGui.QKeySequence(shortcut), True)

            self.acceptShortcutChanges(False)
            return True

    def saveActions(self):
        """ \internal.
            Saves in the current json file a list of pair 
            {actionName, shortcut} of the current actions. 
        """
        # If no filename, call save as.
        if not self.filename and not self.saveActionsAs():
            return

        self.acceptShortcutChanges(False)

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

        ext = ".json"
        fname = self.filename
        
        if not fname:
            fname = str(self.canvasWindow.settings.value("hotkeyEditor/lastFolder"))
        fname, _ = QtGui.QFileDialog.getSaveFileName(self, "Save Hotkey file", fname, str("*" + ext))
            
        if not fname:
            return False

        # Pyside QFileDialog bug on linux.
        # Extension is not added by default.
        if not fname.endswith(ext):
            fname += ext

        print "saveActionsAs " + str(fname)

        self.canvasWindow.settings.setValue("hotkeyEditor/lastFolder", os.path.dirname(fname))
        self.filename = fname

        self.saveActions()
  