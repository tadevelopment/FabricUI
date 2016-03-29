
from PySide import QtCore, QtGui
from SHBaseMenu import SHBaseMenu
from SHLightsMenu import SHLightsMenu
from SHAssetsMenu import SHAssetsMenu

class SHContextualMenu(SHBaseMenu):

  def __init__(self, scene, title):
    super(SHContextualMenu, self).__init__(scene)
    self.setWindowTitle(str(title))
    self.setMouseTracking(True)
    self.setWindowFlags(QtCore.Qt.WindowStaysOnTopHint)

  def _constructMenu(self):
    self.assetMenu = SHAssetsMenu(self.shGLScene, "Assets")
    self.lightsMenu = SHLightsMenu(self.shGLScene, "lights")
    self.addMenu(self.assetMenu)
    self.addMenu(self.lightsMenu)
