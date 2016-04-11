
from PySide import QtCore, QtGui

class SHBaseMenu(QtGui.QMenu):

  def __init__(self, scene, title = None, parent = None):
    self.shGLScene = scene
    super(SHBaseMenu, self).__init__(parent)
    if title is not None : self.setTitle(title)
    self._constructMenu()
    
  def onSceneUpdated(scene):
    self.shGLScene = scene

  def _constructMenu(self):
    pass

