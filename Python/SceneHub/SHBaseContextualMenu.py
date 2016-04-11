
from PySide import QtCore, QtGui
from SHBaseMenu import SHBaseMenu
from SHLightsMenu import SHLightsMenu
from SHAssetsMenu import SHAssetsMenu

class SHBaseContextualMenu(SHBaseMenu):

  def __init__(self, scene, title):
    super(SHBaseContextualMenu, self).__init__(scene)
    self.setWindowTitle(str(title))
    self.setMouseTracking(True)
    self.setWindowFlags(QtCore.Qt.WindowStaysOnTopHint)
 
