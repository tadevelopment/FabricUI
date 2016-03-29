
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from SHBaseMenu import SHBaseMenu

class SHAssetsMenu(SHBaseMenu):

  def __init__(self, scene, title, parent = None):
    super(SHAssetsMenu, self).__init__(scene, title, parent)
 
  def _constructMenu(self):
    addArchiveAction = self.addAction("Add Asset")
    addArchiveExpandAction = self.addAction("Add Asset (Force Expand")
    addArchiveAction.triggered.connect(self.addArchive)
    addArchiveExpandAction.triggered.connect(self.addArchive)
 
  def addArchive(self):   
    dialog = QtGui.QFileDialog(self)
    dialog.setFileMode(QtGui.QFileDialog.ExistingFiles)
    dialog.setNameFilter("Files (*.abc *.fbx)")
    if dialog.exec_():
      pathList = Util.StringUtils.ProcessPathQStringForOsX(dialog.selectedFiles())
      self.shGLScene.addExternalFileList(pathList, self.sender().text() != "Add Asset")
  