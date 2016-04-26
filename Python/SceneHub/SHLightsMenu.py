
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.SceneHub.SHBaseSceneMenu import SHBaseSceneMenu

class SHLightsMenu(SHBaseSceneMenu):

    """SHLightsMenu

    SHLightsMenu specializes SHBaseSceneMenu to add lights to the current SceneHub.SHGLScene.

    Arguments:
        shGLScene (SceneHub.SHGLScene): A reference to a SHGLScene.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, shGLScene, parent = None):
        super(SHLightsMenu, self).__init__(shGLScene, "Lights", parent)
 
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

        if(self.sender().text() == "Add Point"): self.shGLScene.addLight(0)
        elif(self.sender().text() == "Add Spot"): self.shGLScene.addLight(1)
        elif(self.sender().text() == "Add Directional"): self.shGLScene.addLight(2)
        elif(self.sender().text() == "Add Shadow Point"): self.shGLScene.addLight(3)
        elif(self.sender().text() == "Add Shadow Spot"): self.shGLScene.addLight(4)
        elif(self.sender().text() == "Add Shadow Directional"): self.shGLScene.addLight(5)
  