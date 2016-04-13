from FabricEngine import Core, FabricUI, Util, CAPI
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import *
from FabricEngine.Util import *
from FabricEngine.CAPI import *
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
      item = SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index)
      sgObjectVal = pyObjectToRTVal( self.getClient().getContext(), item.getSGObject())

      if sgObjectVal.isValid():
        viewIndexTarget = SceneHub.SHTreeView_ViewIndexTarget(self, index, menu)
        expandAction = QtGui.QAction("Expand recursively", menu)
        expandAction.triggered.connect(viewIndexTarget.expandRecursively)
        menu.addAction(expandAction)
        loadAction = QtGui.QAction("Load recursively", menu)
        loadAction.triggered.connect(viewIndexTarget.loadRecursively)
        menu.addAction(loadAction)

        visible = False
        propagVal = RTVal_ConstructUInt8(self.getClient(), 0)
        visible = sgObjectVal.callMethod("Boolean", "getVisibility", 1, propagVal).getBoolean()
        propagType = propagVal.getUInt8()
    
        
        visMenu = menu.addMenu("Visibility")
        visAction = QtGui.QAction("Show", visMenu)
        visAction.triggered.connect(viewIndexTarget.showLocal)
        visAction.setCheckable( True )
        if visible and not propagType:
          visAction.setChecked( True )
        visMenu.addAction( visAction )

        visAction = QtGui.QAction("Show (propagated)", visMenu)
        visAction.triggered.connect(viewIndexTarget.showPropagated)
        visAction.setCheckable( True )
        if visible and propagType:
          visAction.setChecked(True)
        visMenu.addAction( visAction )

        # THERE ARE BUGS WITH OVERRIDES
        #visAction = new QAction( "Show (override)", 0 )
        #connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( showOverride() ) )
        #visAction.setCheckable( True )
        #if( visible and propagType == 2 )
        #  visAction.setChecked( True )
        #visMenu.addAction( visAction )
        
        visAction = QtGui.QAction("Hide", visMenu)
        visAction.triggered.connect(viewIndexTarget.hideLocal)
        visAction.setCheckable( True )
        if not visible and not propagType:
          visAction.setChecked( True )
        visMenu.addAction( visAction )

        visAction = QtGui.QAction("Hide (propagated)", visMenu)
        visAction.triggered.connect(viewIndexTarget.hidePropagated)
        visAction.setCheckable( True )
        if not visible and propagType:
          visAction.setChecked( True )
        visMenu.addAction( visAction )
        
        # THERE ARE BUGS WITH OVERRIDES
        #visAction = new QAction( "Hide (override)", 0 )
        #connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( hideOverride() ) )
        #visAction.setCheckable( True )
        #if( !visible and propagType == 2 )
        #  visAction.setChecked( True )
        #visMenu.addAction( visAction )
        
 
    else:
      menu.addMenu(SHAssetsMenu(self.shGLScene))
      menu.addMenu(SHLightsMenu(self.shGLScene))
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
        drag = QtGui.QDrag(self)
        drag.setMimeData(mimeData)
        drag.exec_(QtCore.Qt.CopyAction)

    super(SHTreeView, self).mousePressEvent(event)
