
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.SceneHub.SHBaseMenu import SHBaseMenu

class SHLightsMenu(SHBaseMenu):

  def __init__(self, scene, title = None, parent = None):
    super(SHLightsMenu, self).__init__(scene, "Lights", parent)
 
  def _constructMenu(self):
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
    # Get the light we want to add
    if(self.sender().text() == "Add Point"): self.shGLScene.addLight(0)
    elif(self.sender().text() == "Add Spot"): self.shGLScene.addLight(1)
    elif(self.sender().text() == "Add Directional"): self.shGLScene.addLight(2)
    elif(self.sender().text() == "Add Shadow Point"): self.shGLScene.addLight(3)
    elif(self.sender().text() == "Add Shadow Spot"): self.shGLScene.addLight(4)
    elif(self.sender().text() == "Add Shadow Directional"): self.shGLScene.addLight(5)
  