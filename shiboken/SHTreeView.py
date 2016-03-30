from FabricEngine import Core, FabricUI
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import *
from SHAssetsMenu import SHAssetsMenu
from SHLightsMenu import SHLightsMenu
from SHContextualMenu import SHContextualMenu

class SHTreeView(SceneHub.SHBaseTreeView):
  itemSelected = QtCore.Signal(SceneHub.SHTreeItem)
  itemDeselected = QtCore.Signal(SceneHub.SHTreeItem)
  
  def __init__(self, client, scene):
    super(SHTreeView, self).__init__(client)
    self.shGLScene = scene
    self.setHeaderHidden(True)
    self.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
    self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
    self.customContextMenuRequested.connect(self.onCustomContextMenu)
  
  def onCustomContextMenu(self, point):
    menu = QtGui.QMenu(self)
    index = self.indexAt(point)
    if index.isValid():
      viewIndexTarget = SceneHub.SHTreeView_ViewIndexTarget(self, index, menu)
      expandAction = QtGui.QAction("Expand recursively", menu)
      expandAction.triggered.connect(viewIndexTarget.expandRecursively)
      menu.addAction(expandAction)
      loadAction = QtGui.QAction("Load recursively", menu)
      loadAction.triggered.connect(viewIndexTarget.loadRecursively)
      menu.addAction(loadAction)
    else:
      assetMenu = SHAssetsMenu(self.shGLScene)
      lightsMenu = SHLightsMenu(self.shGLScene)
      menu.addMenu(assetMenu)
      menu.addMenu(lightsMenu)
    menu.popup(self.mapToGlobal(point))
  
  def selectionChanged(self, selected, deselected):
    super(SHTreeView, self).selectionChanged(selected, deselected)
    for index in deselected.indexes(): self.itemDeselected.emit(SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index))
    for index in selected.indexes(): self.itemSelected.emit(SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index))
 
  def mousePressEvent(self, event):
    if event.button() == QtCore.Qt.LeftButton:
      urlsList = []
      for index in self.selectedIndexes():
        url = self.shGLScene.getTreeItemPath(SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index))
        if url != "none": 
          print "url " + str(url)
          urlsList.append(QtCore.QUrl(url))

      if len(urlsList) > 0:
        mimeData = QtCore.QMimeData()
        mimeData.setUrls(urlsList)
        # Create drag
        drag = QtGui.QDrag(self);
        drag.setMimeData(mimeData)
        drag.exec_(QtCore.Qt.CopyAction)

    super(SHTreeView, self).mousePressEvent(event)
