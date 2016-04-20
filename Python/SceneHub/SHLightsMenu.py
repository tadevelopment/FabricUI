
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from SHBaseMenu import SHBaseMenu

class SHLightsMenu(SHBaseMenu):

  def __init__(self, shStates, title = None, parent = None):
    super(SHLightsMenu, self).__init__(shStates, "Lights", parent)
 
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
    scene = self.getScene()
    if(self.sender().text() == "Add Point"): scene.addLight(0)
    elif(self.sender().text() == "Add Spot"): scene.addLight(1)
    elif(self.sender().text() == "Add Directional"): scene.addLight(2)
    elif(self.sender().text() == "Add Shadow Point"): scene.addLight(3)
    elif(self.sender().text() == "Add Shadow Spot"): scene.addLight(4)
    elif(self.sender().text() == "Add Shadow Directional"): scene.addLight(5)
  