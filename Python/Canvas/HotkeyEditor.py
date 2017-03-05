#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import re, json, os
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Commands, Actions, Dialog
from FabricEngine.Canvas.CommandManager import *
from FabricEngine.Canvas.CommandRegistry import *
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
from FabricEngine.Canvas.Utils import *

class HotkeyTableWidgetItemDelegate(QtGui.QStyledItemDelegate):

    """ Used to override the event in QTableWidgetItem.    
    """

    keyPressed = QtCore.Signal(QtGui.QKeySequence)

    def __init__(self, parent=None):
        super(HotkeyTableWidgetItemDelegate, self).__init__(parent)

    def createEditor(self, parent, option, index):
        self.editor = QtGui.QLineEdit(parent)
        self.editor.setFrame(False)
        self.editor.installEventFilter(self)
        return self.editor

    def setEditorData(self, editor, index):
        value = index.model().data(index, QtCore.Qt.EditRole)
        editor.setText(value)

    def setModelData(self, editor, model, index):
        value = editor.text()
        model.setData(index, value, QtCore.Qt.EditRole)

    def updateEditorGeometry(self, editor, option, index):
        editor.setGeometry(option.rect)

    def eventFilter(self, target, event):
        if target is self.editor:
            if event.type() == QtCore.QEvent.KeyPress:
                # Gets the sequence from the event.
                keySequence = GetQKeySequenceFromQKeyEvent(event)
                if keySequence is not None:
                    self.keyPressed.emit(keySequence)

                return True  

            if event.type() == QtCore.QEvent.MouseButtonPress:
                return True 

            if event.type() == QtCore.QEvent.MouseButtonDblClick:
                return True  

            if event.type() == QtCore.QEvent.MouseMove:
                return True

        return False    

class CommandAction(QtGui.QAction):
    """ CommandAction associates a command to 
        an action, both have the same name.
        The action has
    """

    def __init__(self, parent, cmdName, shortcut, tooltip):
        """ Initializes a CommandAction.
            Arguments:
            - parent: A reference to the parent
            - cmdName: Name of the command
            - shortcut: Shortcut triggering the command
            - tooltip: Action's tooltip
        """

        super(CommandAction, self).__init__(parent)  
         
        self.setText(cmdName)
        self.setToolTip(tooltip)
        self.triggered.connect(self.onTriggered)
        self.setShortcutContext(QtCore.Qt.ApplicationShortcut)

        # Register the action so an item
        # is created in the HotkeyTable.
        Actions.ActionRegistry.GetActionRegistry().registerAction(cmdName, self)

    def onTriggered(self):
        """ \internal.
            Implementation of Actions.BaseAction.
            Create the command.
        """
        try:
            GetCommandManager().createCommand(self.text())
        except Exception as e:    
            print str(e)

class HotkeyTable(QtGui.QTableWidget):

    """ HotkeyTable is used to associate a shortcut to an action/command.
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
        """ Initializes the HotkeyTable.
            
            Arguments:
            - parent: A reference to wiget.
            - canvasWindow: A reference the canvasWindow.
        """
        super(HotkeyTable, self).__init__(parent)

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
 
        # Construct the delegate
        delegate = HotkeyTableWidgetItemDelegate()
        delegate.keyPressed.connect(self.__onSetItemKeySequence)
        self.setItemDelegate(delegate)

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
        self.setObjectName('HotkeyTable')
        self.setStyleSheet(LoadFabricStyleSheet('FabricUI.qss'))
 
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
        super(HotkeyTable, self).mousePressEvent(event)
        item = self.__getCurrentShortcutItem()
        if item is not None:
            self.editItem(item)

    def mouseDoubleClickEvent(self, event):
        """ Implementation of QtGui.QTableWidget.
            Do nothing.
        """
        pass

    def __createNewRow(self, actionName, shortcut, tooltip = ''):
        """ \internal.
            Create a new row [actionName, shortcut].
        """

        rowCount = self.rowCount() 
        self.insertRow(rowCount)
      
        item = QtGui.QTableWidgetItem(actionName) 
        
        item.setToolTip(tooltip)
        item.setFlags(QtCore.Qt.NoItemFlags)
        self.setItem(rowCount, 0, item)

        item = QtGui.QTableWidgetItem(shortcut)
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
            tooltip = GetCommandRegistry().createCommand(cmdName).getHelp()
            # Add the action to the canvasWindow so it's available.
            # Actions of hidden widgets are not triggered.
            self.canvasWindow.addAction(CommandAction(self, cmdName, QtGui.QKeySequence(), tooltip))
 
    def __onActionRegistered(self, actionName, action):
        """ \internal.
            Called when an action has been registered in Actions.ActionRegistry.
            Create an item associated to the action. 
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        # Check it's the first time the action is registered.
        if actionRegistry.getRegistrationCount(actionName) == 1:
                
            # Get the shorcut, from the action or the json data.
            shortcut = self.__getShortcutFromJSON(actionName)
            if shortcut:
                actionRegistry.setShortcut(actionName, QtGui.QKeySequence(shortcut))
            else:
                shortcut = action.shortcut().toString(QtGui.QKeySequence.NativeText)
            
            self.__createNewRow(actionName, shortcut, action.toolTip())

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
        
        if  (   keySequence != QtGui.QKeySequence() and 
                keySequence != QtGui.QKeySequence('Del') and 
                keySequence != QtGui.QKeySequence('Backspace')  ):

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

            if  (   ( searchByShortcut and regex.search(shortCut.lower()) ) or 
                    ( not searchByShortcut and regex.search(actionName.lower()) ) ):

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
            message.warning(self, 'Hotkey editor', 'Changes not saved' )
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

  
class BaseHotkeyEditorAction(Actions.BaseAction):
 
    def __init__(self, hotkeyEditor, name, text, shortcut):

        self.hotkeyEditor = hotkeyEditor

        super(BaseHotkeyEditorAction, self).__init__(
            hotkeyEditor, 
            name, 
            text, 
            shortcut, 
            QtCore.Qt.ApplicationShortcut)
 
class OpenHotkeyFileAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(OpenHotkeyFileAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.OpenHotkeyFileAction", 
            "Open", 
            QtGui.QKeySequence(""))

        self.setToolTip('Open a Hotkey file')
        
    def onTriggered(self):
        if self.hotkeyEditor.hotkeyTable.openActions():
            basename = os.path.basename(self.hotkeyEditor.hotkeyTable.filename)
            self.hotkeyEditor.setWindowTitle('Hotkey Editor: ' + basename)

class SaveHotkeyFileAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(SaveHotkeyFileAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.SaveHotkeyFileAction", 
            "Save", 
            QtGui.QKeySequence(""))

        self.setToolTip('Save a Hotkey file')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.saveActions()
 
class SaveHotkeyFileAsAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(SaveHotkeyFileAsAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.SaveHotkeyFileAsAction", 
            "Save As", 
            QtGui.QKeySequence(""))

        self.setToolTip('Save a Hotkey file as')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.saveActionsAs()
 
class AcceptActionChanges(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(AcceptActionChanges, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.AcceptActionChanges", 
            "Ok", 
            QtGui.QKeySequence(""))

        self.setToolTip('Accept a Hotkey changes')
        
    def onTriggered(self):
        self.hotkeyEditor.accept()

class RejectActionChanges(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(RejectActionChanges, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.RejectActionChanges", 
            "Cancel", 
            QtGui.QKeySequence(""))

        self.setToolTip('Reject a Hotkey changes')
        
    def onTriggered(self):
        self.hotkeyEditor.reject()

class HotkeyEditor(QtGui.QDialog):

    def __init__(self, canvasWindow):
        """ Initializes the HotkeyEditor.
            Arguments:
            - canvasWindow: A reference to the canvasWindow widget.
        """

        super(HotkeyEditor, self).__init__(canvasWindow) 
        
        # Controls
        comboBoxLabel = QtGui.QLabel('Set')
        self.__comboBox = QtGui.QComboBox(self)
        self.__comboBox.addItem('All')
        self.__comboBox.addItem('Actions')
        self.__comboBox.addItem('Commands')
        self.__comboBox.currentIndexChanged.connect(self.__onFilterItems)

        lineEditLabel =  QtGui.QLabel('Search')
        self.__lineEdit = QtGui.QLineEdit(self)
        self.__lineEdit.textChanged.connect(self.__onFilterItems)

        ctrlLayout = QtGui.QHBoxLayout()
        ctrlLayout.addWidget(lineEditLabel)
        ctrlLayout.addWidget(self.__lineEdit)
        ctrlLayout.addWidget(comboBoxLabel)
        ctrlLayout.addWidget(self.__comboBox)
        
        # HotkeyTable
        self.hotkeyTable = HotkeyTable(self, canvasWindow)
 
        # Toolbar
        openHotkeyFileAction = OpenHotkeyFileAction(self)
        saveHotkeyFileAction = SaveHotkeyFileAction(self)
        saveHotkeyFileAsAction = SaveHotkeyFileAsAction(self)

        acceptActionChanges = AcceptActionChanges(self)
        rejectActionChanges = RejectActionChanges(self)

        toolBar = QtGui.QToolBar()
        toolBar.addAction(openHotkeyFileAction)
        toolBar.addAction(saveHotkeyFileAction)
        toolBar.addAction(saveHotkeyFileAsAction)
        toolBar.addSeparator()
        toolBar.addAction(acceptActionChanges)
        toolBar.addAction(rejectActionChanges)
    
        # All
        layout = QtGui.QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(toolBar)
        layout.addLayout(ctrlLayout)
        layout.addWidget(self.hotkeyTable)
        self.setLayout(layout)

        # qss
        self.setWindowTitle('Hotkey Editor')
        self.setObjectName('HotkeyEditor')
        self.setStyleSheet(LoadFabricStyleSheet('FabricUI.qss'))

        # !!!! To change
        self.setMinimumHeight(800)
        self.setMinimumWidth(1200)
        self.adjustSize()
 
    def __onFilterItems(self):
        """ \internal.
            Filter the actions.
        """
        self.hotkeyTable.filterItems(self.__lineEdit.text(), self.__comboBox.currentText())

    def accept(self):
        """ \internal.
            Implementation of QtGui.QDialog.
        """
        self.hotkeyTable.acceptShortcutChanges()
        super(HotkeyEditor, self).accept()

    def reject(self):
        """ \internal.
            Implementation of QtGui.QDialog.
        """
        self.hotkeyTable.rejectShortcutChanges()

        super(HotkeyEditor, self).reject()
 