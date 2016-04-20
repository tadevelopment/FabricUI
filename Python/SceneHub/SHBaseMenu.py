
from PySide import QtCore, QtGui

class SHBaseMenu(QtGui.QMenu):

  def __init__(self, shStates, title = None, parent = None):
    self.shStates = shStates
    super(SHBaseMenu, self).__init__(parent)
    if title is not None : self.setTitle(title)
    self._constructMenu()
    
  def getScene(self):
    return self.shStates.getActiveScene()

  def _constructMenu(self):
    pass

