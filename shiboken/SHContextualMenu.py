
from PySide import QtCore, QtGui
from SHBaseContextualMenu import SHBaseContextualMenu
from SHLightsMenu import SHLightsMenu
from SHAssetsMenu import SHAssetsMenu

class SHContextualMenu(SHBaseContextualMenu):

  def __init__(self, scene):
    super(SHContextualMenu, self).__init__(scene, "")
 
  def _constructMenu(self):
    self.assetMenu = SHAssetsMenu(self.shGLScene)
    self.lightsMenu = SHLightsMenu(self.shGLScene)
    self.addMenu(self.assetMenu)
    self.addMenu(self.lightsMenu)
