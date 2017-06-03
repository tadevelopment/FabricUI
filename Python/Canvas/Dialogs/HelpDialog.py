#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import os
from PySide import QtCore, QtGui
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.HotkeyEditor.HotKeyEditorActions import *
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableWidget import HotkeyTableWidget
 
class HelpDialog(QtGui.QDialog):

    def __init__(self, title, text = "", parent = None):
        """ Initializes the HelpDialog.
            Arguments:
            - canvasWindow: A reference to the canvasWindow widget.
        """

        super(HelpDialog, self).__init__(parent) 
 
        self.setWindowTitle(title)
        self.setObjectName('HelpDialog_' + title)
        self.setStyleSheet(LoadFabricStyleSheet('FabricUI.qss'))
        
        self.text = QtGui.QTextEdit(text, self)

        # All
        layout = QtGui.QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(self.text)
        self.setLayout(layout)

        # !!!! To change
        self.setMinimumHeight(350)
        self.setMinimumWidth(550)
        self.adjustSize()