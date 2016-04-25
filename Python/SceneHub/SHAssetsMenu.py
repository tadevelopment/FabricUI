
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.SceneHub.SHBaseMenu import SHBaseMenu

class SHAssetsMenu(SHBaseMenu):

    """SHAssetsMenu

    SHAssetsMenu is a specialization of SHBaseMenu.
    It supports : importing Alembic/Fbx archives within the SceneHub.SHGLScene scene.
                  exporting the scene to an Alembic archive.

    Arguments:
        shGLScene (SceneHub.SHGLScene): A reference to a shGLScene.
        title (String): The menu title, can be None.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, shGLScene, title = None, parent = None):
        super(SHAssetsMenu, self).__init__(shGLScene, "Assets", parent)
     
    def constructMenu(self):
        addAction = self.addAction("Add Asset")
        addExpandAction = self.addAction("Add Asset (Force Expand)")
        self.addSeparator()
        exportAlembicAction = self.addAction("Export to Alembic")

        addAction.triggered.connect(self.addArchive)
        addExpandAction.triggered.connect(self.addArchive)
        exportAlembicAction.triggered.connect(self.exportToAlembic)

    def addArchive(self):   
        """Import an archive (alembic or fbx) in the current scene.
        """
        dialog = QtGui.QFileDialog(self)
        dialog.setFileMode(QtGui.QFileDialog.ExistingFiles)
        dialog.setNameFilter("Files (*.abc *.fbx)")
        if dialog.exec_():
            pathList = Util.StringUtils.ProcessPathQStringForOsX(dialog.selectedFiles())
            self.shGLScene.addExternalFileList(pathList, self.sender().text() != "Add Asset")
      
    def exportToAlembic(self):   
        """Exports the current scene to alembic.
        """
        fileName = QtGui.QFileDialog.getSaveFileName(self, "Export to Alembic", "", "Files (*.abc)")
        if dialog.exec_():
            pathList = []
            pathList.append(fileName)
            pathList = Util.StringUtils.ProcessPathQStringForOsX(pathList)
            self.shGLScene.exportToAlembic(pathList[0])
      