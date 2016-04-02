
from PySide import QtCore, QtGui
from SHBaseContextualMenu import SHBaseContextualMenu
from SHLightsMenu import SHLightsMenu
from SHAssetsMenu import SHAssetsMenu

class SHContextualMenu(SHBaseContextualMenu):

  def __init__(self, scene):
    super(SHContextualMenu, self).__init__(scene, "")
 
  def _constructMenu(self):
    assetMenu = SHAssetsMenu(self.shGLScene)
    lightsMenu = SHLightsMenu(self.shGLScene)
    self.addMenu(assetMenu)
    self.addMenu(lightsMenu)
