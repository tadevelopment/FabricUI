
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.FabricUI import Menus

class SHInteractionMenu(Menus.BaseMenu):

    """SHAssetsMenu

    SHAssetsMenu specializes BaseMenu for importing/exporting assets in a SceneHub.SHGLScene.
    It supports : importing Alembic/Fbx archives within scene.
                  exporting the scene to an Alembic archive.

    Arguments:
        shGLRenderer (SceneHub.SHGLRenderer): A reference to a SHGLRenderer.
        parent (QWidget): The menu widget parent, can be None.

    """

    togglePlayback = QtCore.Signal()

    def __init__(self, shGLRenderer, parent = None):
        self.shGLRenderer = shGLRenderer
        super(SHInteractionMenu, self).__init__(self.shGLRenderer.getClient(), "Tools", parent)
        self.aboutToShow.connect(self.constructMenu)
        self.constructMenu()

    def constructMenu(self):
        """Implementation of BaseMenu.
        """

        self.clear();
        tools = self.shGLRenderer.getRegisteredTools()
        for i in range(0, len(tools[0])):
            toolAction = self.addAction(tools[0][i] + "\t" + tools[1][i])
            toolAction.triggered.connect(self.onActiveTool)
        
    def onActiveTool(self):
        """Activates the selected tool.
        """

        # From the sender action text : ToolName \tab ToolKey
        # gets the toolKey only
        action = self.sender()
        actionName = str(action.text())

        pos = actionName.find("\t")     
        toolName = actionName[0:pos] 
        toolKey = actionName[pos:len(actionName)] 
        toolKey = toolKey.strip()

        #From the toolKey, create a fake key event and process it
        seq = QtGui.QKeySequence(toolKey)
        qtKey = QtGui.QKeyEvent(QtCore.QEvent.KeyPress, seq[0], QtCore.Qt.NoModifier)
         
        redrawAllViewports = bool()
        #Viewport 0
        self.shGLRenderer.onEvent(0, qtKey, redrawAllViewports, False)
     