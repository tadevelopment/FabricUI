
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from SHBaseMenu import SHBaseMenu

class SHAssetsMenu(SHBaseMenu):

  def __init__(self, scene, title = None, parent = None):
    super(SHAssetsMenu, self).__init__(scene, "Assets", parent)
 
  def _constructMenu(self):
    addAction = self.addAction("Add Asset")
    addExpandAction = self.addAction("Add Asset (Force Expand)")
    self.addSeparator()
    exportAlembicAction = self.addAction("Export to Alembic")

    addAction.triggered.connect(self.addArchive)
    addExpandAction.triggered.connect(self.addArchive)
    exportAlembicAction.triggered.connect(self.exportToAlembic)

  def addArchive(self):   
    dialog = QtGui.QFileDialog(self)
    dialog.setFileMode(QtGui.QFileDialog.ExistingFiles)
    dialog.setNameFilter("Files (*.abc *.fbx)")
    if dialog.exec_():
      pathList = Util.StringUtils.ProcessPathQStringForOsX(dialog.selectedFiles())
      self.shGLScene.addExternalFileList(pathList, self.sender().text() != "Add Asset")
  
  def exportToAlembic(self):   
    fileName = QtGui.QFileDialog.getSaveFileName(self, "Export to Alembic", "", "Files (*.abc)")
    if dialog.exec_():
      pathList = []
      pathList.append(fileName)
      pathList = Util.StringUtils.ProcessPathQStringForOsX(pathList)
      self.shGLScene.exportToAlembic(pathList[0])
  