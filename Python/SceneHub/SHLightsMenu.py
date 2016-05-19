
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.FabricUI import SceneHub

class SHLightsMenu(SceneHub.SHBaseSceneMenu):

    """SHLightsMenu

    SHLightsMenu specializes SHBaseSceneMenu to add lights to the current SceneHub.SHGLScene.

    Arguments:
        shGLScene (SceneHub.SHGLScene): A reference to a SHGLScene.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, shGLScene, parent = None):
        super(SHLightsMenu, self).__init__(shGLScene, "Lights", parent)
        self.constructMenu()

    def constructMenu(self):
        """Implementation of BaseMenu.
        """
        
        addSpotLightAction = self.addAction("Add Spot")
        addSpotLightAction.triggered.connect(self.addLight)
        addPointLightAction = self.addAction("Add Point")
        addPointLightAction.triggered.connect(self.addLight)
        addDirectLightAction = self.addAction("Add Directional")
        addDirectLightAction.triggered.connect(self.addLight)
        addShadowSpotLightAction = self.addAction("Add Shadow Spot")
        addShadowSpotLightAction.triggered.connect(self.addLight)
        addShadowPointLightAction = self.addAction("Add Shadow Point")
        addShadowPointLightAction.triggered.connect(self.addLight)
        addShadowDirectLightAction = self.addAction("Add Shadow Directional")
        addShadowDirectLightAction.triggered.connect(self.addLight)
    
    def addLight(self):   
        """Adds a light to the current scene.
        """

        if(self.sender().text() == "Add Point"): self.m_shGLScene.addLight(0)
        elif(self.sender().text() == "Add Spot"): self.m_shGLScene.addLight(1)
        elif(self.sender().text() == "Add Directional"): self.m_shGLScene.addLight(2)
        elif(self.sender().text() == "Add Shadow Point"): self.m_shGLScene.addLight(3)
        elif(self.sender().text() == "Add Shadow Spot"): self.m_shGLScene.addLight(4)
        elif(self.sender().text() == "Add Shadow Directional"): self.m_shGLScene.addLight(5)
  