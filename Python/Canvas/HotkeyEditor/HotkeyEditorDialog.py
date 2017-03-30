#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine.FabricUI import Actions
from FabricEngine.Canvas.HotkeyEditor.HotKeyEditorActions import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableWidget import HotkeyTableWidget
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
  
class HotkeyEditorDialog(QtGui.QDialog):

    def __init__(self, canvasWindow):
        """ Initializes the HotkeyEditorDialog.
            Arguments:
            - canvasWindow: A reference to the canvasWindow widget.
        """

        super(HotkeyEditorDialog, self).__init__(canvasWindow) 
        
         # qss
        self.setWindowTitle('Hotkey Editor')
        self.setObjectName('HotkeyEditorDialog')
        self.setStyleSheet(LoadFabricStyleSheet('FabricUI.qss'))

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
        
        # HotkeyTableWidget
        self.hotkeyTableWidget = HotkeyTableWidget(self, canvasWindow)
 
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
        layout.addWidget(self.hotkeyTableWidget)
        self.setLayout(layout)

       
        # !!!! To change
        self.setMinimumHeight(400)
        self.setMinimumWidth(600)
        self.adjustSize()
 
    def __onFilterItems(self):
        """ \internal.
            Filter the actions.
        """
        self.hotkeyTableWidget.filterItems(self.__lineEdit.text(), self.__comboBox.currentText())

    def accept(self):
        """ \internal.
            Implementation of QtGui.QDialog.
        """
        self.hotkeyTableWidget.acceptShortcutChanges()
        super(HotkeyEditorDialog, self).accept()

    def reject(self):
        """ \internal.
            Implementation of QtGui.QDialog.
        """
        self.hotkeyTableWidget.rejectShortcutChanges()

        super(HotkeyEditorDialog, self).reject()
 