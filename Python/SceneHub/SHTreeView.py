from FabricEngine import Core, FabricUI, Util, CAPI
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import *
from FabricEngine.Util import *
from FabricEngine.CAPI import *
from FabricEngine.SceneHub.SHAssetsMenu import SHAssetsMenu
from FabricEngine.SceneHub.SHLightsMenu import SHLightsMenu
from FabricEngine.SceneHub.SHContextualMenu import SHContextualMenu

class SHTreeView(SceneHub.SHBaseTreeView):
  selectionCleared = QtCore.Signal()
  itemSelected = QtCore.Signal(SceneHub.SHTreeItem)
  itemDeselected = QtCore.Signal(SceneHub.SHTreeItem)
  
  def __init__(self, client, shStates, scene):
    super(SHTreeView, self).__init__(client)
    self.client = client # Keep a ref: the C++ wrapped one from the parent doesn't have .RT
    self.shGLScene = scene
    self.shStates = shStates
    self.setHeaderHidden(True)
    self.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
    self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
    self.customContextMenuRequested.connect(self.onCustomContextMenu)
  
  def onCustomContextMenu(self, point):
    index = self.indexAt( point );
    item = SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index)
    sgObject = None
    if item: 
      sgObject = item.getSGObject()   
    menu = SHContextualMenu(self.client, self.shGLScene, self.shStates, sgObject)
    menu.exec_(self.mapToGlobal(point))
  
  def selectionChanged(self, selected, deselected):
    # clear selection (make sure 3D view is synchronized) if all elements are newly added
    clear = len(self.selectionModel().selectedIndexes()) == len(selected.indexes())

    super(SHTreeView, self).selectionChanged(selected, deselected)
    for index in deselected.indexes(): self.itemDeselected.emit(SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index))
    if clear:
      self.selectionCleared.emit()
    for index in selected.indexes(): self.itemSelected.emit(SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index))
 
  def mousePressEvent(self, event):
    if event.button() == QtCore.Qt.LeftButton:
      urlsList = []
      for index in self.selectedIndexes():
        item = SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index)
        sgObj = item.getSGObject()
        if sgObj is not None:
          url = self.shGLScene.getTreeItemPath(item.getSGObject())
          if url is not None: 
            urlsList.append(QtCore.QUrl(url))

      if len(urlsList) > 0:
        mimeData = QtCore.QMimeData()
        mimeData.setUrls(urlsList)
        # Create drag
        drag = QtGui.QDrag(self)
        drag.setMimeData(mimeData)
        drag.exec_(QtCore.Qt.CopyAction)

    super(SHTreeView, self).mousePressEvent(event)

  def mouseDoubleClickEvent(self, event):
    for index in self.selectedIndexes():
      item = SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index)
      if item is not None:
        self.itemDoubleClicked.emit(item)   
