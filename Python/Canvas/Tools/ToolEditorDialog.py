#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine.Canvas.Tools.ToolTableWidget import ToolTableWidget
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
  
class ToolEditorDialog(QtGui.QDialog):

    def __init__(self, canvasWindow):
        """ Initializes the ToolEditorDialog.
        
            Arguments:
            - canvasWindow: A reference to the canvasWindow widget.
        """

        super(ToolEditorDialog, self).__init__(canvasWindow) 
            
        # Controls
        lineEditLabel =  QtGui.QLabel('Search')
        self.__lineEdit = QtGui.QLineEdit(self)
        self.__lineEdit.textChanged.connect(self.__onFilterItems)

        ctrlLayout = QtGui.QHBoxLayout()
        ctrlLayout.addWidget(lineEditLabel)
        ctrlLayout.addWidget(self.__lineEdit)

        # ToolTableWidget
        self.cmdListTableWidget = ToolTableWidget(self, canvasWindow)
    
        # All 
        layout = QtGui.QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addLayout(ctrlLayout)
        layout.addWidget(self.cmdListTableWidget)
        self.setLayout(layout)

        # qss
        self.setWindowTitle('Tool Editor')
        self.setObjectName('ToolEditorDialog')
        self.setStyleSheet(LoadFabricStyleSheet('FabricUI.qss'))

        # !!!! To change
        self.setMinimumHeight(400)
        self.setMinimumWidth(600)
        self.adjustSize()
 
    def __onFilterItems(self):
        """ \internal.
            Filter the actions.
        """
        self.cmdListTableWidget.filterItems(self.__lineEdit.text())
