#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Commands, Actions
from FabricEngine.Canvas.CommandManager import *
from FabricEngine.Canvas.CommandRegistry import *
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
from FabricEngine.Canvas.Utils import *


class QTableWidgetItemDelegate(QtGui.QStyledItemDelegate):

    """ Used to override the event in QTableWidgetItem.    
    """

    keyPressed = QtCore.Signal(str)

    def __init__(self, parent=None):
        super(QTableWidgetItemDelegate, self).__init__(parent)

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

class CommandAction(Actions.BaseAction):
    """ CommandAction specializes Actions.BaseAction (C++).
        It associates a command to the action, both have
        the same name.
    """

    def __init__(self, parent, cmdName, shortcut):
        """ Initializes a CommandAction.
            Arguments:
            - parent: A reference to the parent
            - cmdName: Name of the command
            - shortcut: Shortcut triggering the command
        """
        super(CommandAction, self).__init__(
            parent,
            cmdName,
            cmdName,
            shortcut)  

    def onTriggered(self):
        """ \internal.
            Implementation of Actions.BaseAction
        """
        try:
            GetCommandManager().createCommand(self.text())
        except Exception as e:    
            print str(e)

class HotkeyEditor(QtGui.QTableWidget):

    """ HotkeyEditor is used to associate a shortcut to an action/command.
        When an action or a command is registered by their registry, 
        a signal is sent to the editor so it refreshes its content.
    """

    def __init__(self, parent = None):
        """ Initializes the HotkeyEditor.
            Arguments:
            - parent: A reference parent wiget, can be None.
        """
        super(HotkeyEditor, self).__init__(parent)

        # Notify when an action is registered-
        # unregistered from the Actions.ActionRegistry.
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        actionRegistry.actionRegistered.connect(self.__onActionRegistered)
        actionRegistry.actionUnregistered.connect(self.__onActionUnregistered)

        # Notify when an command is registered.
        GetCommandRegistry().commandRegisteredCallback.connect(self.__onCommandRegistered)
 
        # Construct the tabble
        delegate = QTableWidgetItemDelegate()
        delegate.keyPressed.connect(self.__setActionShortcut)
        self.setItemDelegate(delegate)

        # Two coloums: [actionName, shortcut]
        self.setColumnCount(2)
        self.setHorizontalHeaderItem(0, QtGui.QTableWidgetItem("Action"))
        self.setHorizontalHeaderItem(1, QtGui.QTableWidgetItem("Shortcut"))
        
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragEnabled(False)
        self.setTabKeyNavigation(False)
        self.setSortingEnabled(True)
        self.verticalHeader().setVisible(False)
        self.horizontalHeader().setStretchLastSection(True)

        # qss
        self.setObjectName('ShortcutMenu')
        self.setStyleSheet(LoadFabricStyleSheet("FabricUI.qss"))
 
    def __getCurrentItem(self):
        """ \internal.
        """
        if self.currentColumn() == self.columnCount()-1:
            return self.item(self.currentRow(), self.currentColumn())

    def mousePressEvent(self, event):
        """ Implementation of QtGui.QTableWidget.
            Directly edit the item to simulate a double-click.
        """
        super(HotkeyEditor, self).mousePressEvent(event)
        item = self.__getCurrentItem()
        if item is not None:
            self.editItem(item)

    def mouseDoubleClickEvent(self, event):
        """ Implementation of QtGui.QTableWidget.
            Do nothing.
        """
        pass

    def __createNewRow(self, actionName, shortcut, fromCmd = False):
        """ \internal.
            Create a new row [actionName, shortcut].
        """
        rowCount = self.rowCount() 
        self.insertRow(rowCount)
        lightGrey = QtGui.QColor(109, 191, 200)
        withe = QtGui.QColor(255, 255, 255)

        item = QtGui.QTableWidgetItem(actionName) 
        if fromCmd:
            item.setForeground(lightGrey)
        else:
            item.setForeground(withe)
        item.setFlags(QtCore.Qt.NoItemFlags)
        self.setItem(rowCount, 0, item)

        item = QtGui.QTableWidgetItem(shortcut)
        if fromCmd:
            item.setForeground(lightGrey)
        else:
            item.setForeground(withe)
        self.setItem(rowCount, 1, item)

        # Hack to refresh the view correctly.
        self.setVisible(False);
        self.resizeColumnsToContents();
        self.setVisible(True);

    def __onCommandRegistered(self, cmdName, cmdType, implType):
        """ \internal.
            Called when an command has been registered in CommandRegistry.
            Create an action and an item associated to the command. 
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        if actionRegistry.getAction(cmdName) is None:
            self.addAction(CommandAction(self, cmdName, QtGui.QKeySequence()))
            self.__createNewRow(cmdName, "", True)

    def __onActionRegistered(self, actionName, action):
        """ \internal.
            Called when an action has been registered in Actions.ActionRegistry.
            Create an item associated to the command. 
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        # Check it's the first time the action is reggistered.
        if actionRegistry.getRegistrationCount(actionName) == 1:
            # Check the action is not from a CommandAction.
            # Don't want to have the same item twice.
            if not GetCommandRegistry().isCommandRegistered(actionName):
                self.__createNewRow(actionName, action.shortcut().toString())
    
    def __onActionUnregistered(self, actionName):
        """ \internal.
            Called when an action has been  
            unregistered from Actions.ActionRegistry.
        """
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        
        # Check there is no more action registered under `actionName`.
        if actionRegistry.getRegistrationCount(actionName) == 0:
            # Remove the item.
            items = self.findItems(actionName, QtCore.Qt.MatchExactly)
            if items:
                self.removeRow(items[0].row())

    def __setActionShortcut(self, keySequence):
        """ \internal.
            Sets the shortcut to the action refered by
            the current item when the user press a key.
        """
        item = self.__getCurrentItem()

        if item is not None:
            actionName = self.item(self.currentRow(), 0).text()
            actionRegistry = Actions.ActionRegistry.GetActionRegistry()

            deleteKey = keySequence == QtGui.QKeySequence("Del") or keySequence == QtGui.QKeySequence("Backspace")
            if not deleteKey:
                # Check if an action already used the shortcut.
                # If so, display a warning and return
                otherWithShortcut = actionRegistry.isShortcutUsed(keySequence)
               
                if otherWithShortcut:
                    action = actionRegistry.getAction(actionName) 

                    # Check if the current action or the one.
                    # having the shortcut already is global.
                    isActionGlobal = (action.shortcutContext() == QtCore.Qt.WindowShortcut or  
                                     action.shortcutContext() == QtCore.Qt.ApplicationShortcut) 

                    isOtherWithShortcutGlobal = (otherWithShortcut.shortcutContext() == QtCore.Qt.WindowShortcut or  
                                     otherWithShortcut.shortcutContext() == QtCore.Qt.ApplicationShortcut)

                    # If both actions are local (to their widget),
                    # check if they have the same parent.
                    sharedSameParent = False
                    if not isActionGlobal and not isOtherWithShortcutGlobal:
                        sharedSameParent = otherWithShortcut.parentWidget() == action.parentWidget()

                    if isActionGlobal or isOtherWithShortcutGlobal or sharedSameParent:

                        message = QtGui.QMessageBox()
                        message.warning(self,   
                            'Command Menu', "shorcut " + str(keySequence) + 
                            " already used by " + str(actionRegistry.getActionName(otherWithShortcut)))
                        return  

                # Set the same shortcut to all the  
                # actions registered as `actionName`.
                actionRegistry.setShortcut(actionName, QtGui.QKeySequence(keySequence))

                # Update the item text
                item.setText(keySequence)
                
            else:
                actionRegistry.setShortcut(actionName, QtGui.QKeySequence())
                item.setText("")
