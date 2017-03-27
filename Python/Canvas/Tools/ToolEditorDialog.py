#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine.FabricUI import Actions
from FabricEngine.Canvas.HotkeyEditor.HotKeyEditorActions import *
from FabricEngine.Canvas.HotkeyEditor.HotkeyTableWidget import HotkeyTableWidget
from FabricEngine.Canvas.LoadFabricStyleSheet import LoadFabricStyleSheet
  
class ToolEditorDialog(QtGui.QDialog):

    def __init__(self, canvasWindow):
        """ Initializes the ToolEditorDialog.
            Arguments:
            - canvasWindow: A reference to the canvasWindow widget.
        """

        super(ToolEditorDialog, self).__init__(canvasWindow) 
     
 