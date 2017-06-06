#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import os
from PySide import QtCore, QtGui
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.HotkeyEditor.HotKeyEditorActions import *
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableWidget import HotkeyTableWidget
    
class HotkeyLineEdit(QtGui.QLineEdit):

    updateFocus = QtCore.Signal(bool)

    def __init__(self, parent):
        super(HotkeyLineEdit, self).__init__(parent)

    def focusInEvent(self, event):
        """ Implementation of QtGui.QLineEdit.
        """
        super(HotkeyLineEdit, self).focusInEvent(event) 
        self.updateFocus.emit(True)

    def focusOutEvent(self, event):
        """ Implementation of QtGui.QLineEdit.
        """
        super(HotkeyLineEdit, self).focusOutEvent(event) 
        self.updateFocus.emit(False)

class HotkeyEditorDialog(QtGui.QDialog):

    def __init__(self, canvasWindow):
        """ Initializes the HotkeyEditorDialog.
            Arguments:
            - canvasWindow: A reference to the canvasWindow widget.
        """

        super(HotkeyEditorDialog, self).__init__(canvasWindow) 
        
        self.baseTitle = "Hotkey Editor"  
       
        # qss
        self.setWindowTitle('Hotkey Editor')
        self.setObjectName('HotkeyEditorDialog')
        self.setStyleSheet(LoadFabricStyleSheet('FabricUI.qss'))

       # HotkeyTableWidget
        self.hotkeyTable = HotkeyTableWidget(
            self, 
            canvasWindow
            )

        self.hotkeyTable.manager.stateIsDirty.connect(
            self.onUpdate
            )

        # Controls
        comboBoxLabel = QtGui.QLabel('Set')
        self.__itemComboBox = QtGui.QComboBox(self)
        self.__itemComboBox.addItem('All')
        self.__itemComboBox.addItem('Acts')
        self.__itemComboBox.addItem('Cmds')
        self.__itemComboBox.currentIndexChanged.connect(self.__onFilterItems)

        checkBoxLabel = QtGui.QLabel('Edit.')
        self.__editComboBox = QtGui.QComboBox(self)
        self.__editComboBox.addItem('All')
        self.__editComboBox.addItem('Yes')
        self.__editComboBox.addItem('No')
        self.__editComboBox.currentIndexChanged.connect(self.__onFilterItems)

        lineEditLabel =  QtGui.QLabel('Search')
        lineEditLabel.setToolTip("Trie")
        self.__lineEdit = HotkeyLineEdit(self)
        self.__lineEdit.updateFocus.connect(self.hotkeyTable.onEmitEditingItem)
        self.__lineEdit.textChanged.connect(self.__onFilterItems)

        ctrlLayout = QtGui.QHBoxLayout()
        ctrlLayout.addWidget(lineEditLabel)
        ctrlLayout.addWidget(self.__lineEdit)
        ctrlLayout.addWidget(comboBoxLabel)
        ctrlLayout.addWidget(self.__itemComboBox)
        ctrlLayout.addWidget(checkBoxLabel)
        ctrlLayout.addWidget(self.__editComboBox)
 
        # Toolbar
        openHotkeyFileAction = OpenHotkeyFileAction(self)
        saveHotkeyFileAction = SaveHotkeyFileAction(self)
        saveHotkeyFileAsAction = SaveHotkeyFileAsAction(self)
        acceptActionChanges = AcceptActionChangesAndExit(self)
        rejectActionChanges = RejectActionChangesAndExit(self)
        UndoActionChanges(self)
        RedoActionChanges(self)

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

        # !!!! To change
        self.setMinimumHeight(350)
        self.setMinimumWidth(550)
        self.adjustSize()
    
    def onUpdate(self, isDirty, clear): 
        """ Update the widget.
        """

        dirtyText = ''
        if isDirty:
            dirtyText = "*"

        fileName = self.hotkeyTable.manager.filename
        if fileName:
            baseName = os.path.basename(fileName)
            self.setWindowTitle(self.baseTitle + ' : ' + baseName + dirtyText)
        
        else:
            self.setWindowTitle(self.baseTitle + dirtyText)

        if clear:
            self.hotkeyTable.qUndoStack.clear()

    def __onFilterItems(self):
        """ \internal.
            Filter the actions.
        """

        self.hotkeyTable.filterItems(
            self.__lineEdit.text(), 
            self.__editComboBox.currentIndex(),
            self.__itemComboBox.currentIndex())

    def showEvent(self, event):
        """ Implementation of QtGui.QDialog.
        """
        super(HotkeyEditorDialog, self).showEvent(event)
        GetCommandRegistry().synchronizeKL();

    def closeEvent(self, event):
        """ Implementation of QtGui.QDialog.
        """
        self.hotkeyTable.manager.rejectShortcutChanges()
        super(HotkeyEditorDialog, self).closeEvent(event)