#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine import Core
from FabricEngine.Canvas.Utils import *
from FabricEngine.FabricUI import Commands, ValueEditor
from FabricEngine.Canvas.Commands.CommandRegistry import *

class ToolOptionsEditor(ValueEditor.VETreeWidget):

    """ ToolOptionsEditor  
    """

    valueChanged = QtCore.Signal()

    def __init__(self, parent, canvasWindow):
        """ Initializes the ToolOptionsEditor.
            
            Arguments:
            - parent: A reference to the parent widget.
            - canvasWindow: A reference the canvasWindow.
        """
        super(ToolOptionsEditor, self).__init__(parent)

        self.canvasWindow = canvasWindow
        self.client = canvasWindow,client

        # qss
        self.setObjectName('ToolOptionsEditor')

    def onUpdateOptions(self):
        pass

 