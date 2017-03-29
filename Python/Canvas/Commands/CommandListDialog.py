#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine.Canvas.Commands.CommandListTableWidget import CommandListTableWidget
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
  
class CommandListDialog(QtGui.QDialog):

    def __init__(self, canvasWindow):
        """ Initializes the CommandListDialog.
            Arguments:
            - canvasWindow: A reference to the canvasWindow widget.
        """

        super(CommandListDialog, self).__init__(canvasWindow) 
            
        # CommandListTableWidget
        self.cmdListTableWidget = CommandListTableWidget(self, canvasWindow)
    
        # All 
        layout = QtGui.QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(self.cmdListTableWidget)
        self.setLayout(layout)

        # qss
        self.setWindowTitle('Command List')
        self.setObjectName('CommandListDialog')
        self.setStyleSheet(LoadFabricStyleSheet('FabricUI.qss'))

        # !!!! To change
        self.setMinimumHeight(400)
        self.setMinimumWidth(600)
        self.adjustSize()
 
 