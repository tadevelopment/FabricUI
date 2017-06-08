#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
  
class HelpDialog(QtGui.QDialog):

    def __init__(self, title, text = "", parent = None):
        """ Initializes the HelpDialog.
        """

        super(HelpDialog, self).__init__(parent) 
 
        self.setWindowTitle(title)
        self.setObjectName('HelpDialog_' + title)
        self.setStyleSheet(LoadFabricStyleSheet('FabricUI.qss'))
        
        self.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        self.setContentsMargins(0, 0, 0, 0)

        mainlayout = QtGui.QVBoxLayout()
        self.setLayout( mainlayout )
        mainlayout.setContentsMargins( 5, 5, 5, 5 )
        mainlayout.setSpacing(2)

        self.text = QtGui.QLabel(text, self)
        self.text.setContentsMargins( 0, 0, 0, 0 )
 
        self.text.setSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Minimum);
        self.text.setWordWrap(True);
    
        inputsLayout = QtGui.QVBoxLayout()
        inputsLayout.setContentsMargins( 0, 0, 0, 0 )
        inputsLayout.setSpacing(2)
        self.text.setLayout(inputsLayout)

        mainlayout.addWidget(self.text)
 