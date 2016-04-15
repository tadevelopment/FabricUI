
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI, Util, CAPI
from FabricEngine.FabricUI import SceneHub
from FabricEngine.Util import *
from FabricEngine.CAPI import *
from SHBaseMenu import SHBaseMenu
from SHLightsMenu import SHLightsMenu
from SHAssetsMenu import SHAssetsMenu

class SHContextualMenu(SHBaseMenu):

  def __init__(self, scene, treeView):
    self.treeView = treeView
    super(SHContextualMenu, self).__init__(scene)
 
  def _constructMenu(self):

    client = self.shGLScene.getClient()
    # TreeView Selection
    selectedTreeItemIndexes = self.treeView.selectedIndexes()
    # 3D View Selection
    selectedViewItemIndexes = self.shGLScene.getSelectedObjects()

    if len(selectedTreeItemIndexes) > 0 and len(selectedViewItemIndexes) > 0:
      expandAction = QtGui.QAction("Expand recursively", self)
      self.addAction(expandAction)
      loadAction = QtGui.QAction("Load recursively", self)
      self.addAction(loadAction)

      visMenu = self.addMenu("Visibility")
      showLocalAction = QtGui.QAction("Show", visMenu)
      showLocalAction.setCheckable( True )
      visMenu.addAction( showLocalAction )

      showPropagatedAction = QtGui.QAction("Show (propagated)", visMenu)
      showPropagatedAction.setCheckable( True )
      visMenu.addAction( showPropagatedAction )

      hideLocalAction = QtGui.QAction("Hide", visMenu)
      hideLocalAction.setCheckable( True )
      visMenu.addAction( hideLocalAction )

      hidePropagatedAction = QtGui.QAction("Hide (propagated)", visMenu)
      hidePropagatedAction.setCheckable( True )
      visMenu.addAction( hidePropagatedAction )

      for index in selectedTreeItemIndexes:
        
        if index.isValid():
          item = SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index)
          sgObjectVal = pyObjectToRTVal( client.getContext(), item.getSGObject())

          if sgObjectVal.isValid():
            viewIndexTarget = SceneHub.SHTreeView_ViewIndexTarget(self.treeView, index, self)
           
            expandAction.triggered.connect(viewIndexTarget.expandRecursively)
            loadAction.triggered.connect(viewIndexTarget.loadRecursively)

            visible = False
            propagVal = RTVal_ConstructUInt8(client, 0)
            visible = sgObjectVal.callMethod("Boolean", "getVisibility", 1, propagVal).getBoolean()
            propagType = propagVal.getUInt8()
                
            showLocalAction.triggered.connect(viewIndexTarget.showLocal)
            if visible and not propagType:
              showLocalAction.setChecked( True )

            showPropagatedAction.triggered.connect(viewIndexTarget.showPropagated)
            if visible and propagType:
              showPropagatedAction.setChecked(True)

            # THERE ARE BUGS WITH OVERRIDES
            #visAction = new QAction( "Show (override)", 0 )
            #connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( showOverride() ) )
            #visAction.setCheckable( True )
            #if( visible and propagType == 2 )
            #  visAction.setChecked( True )
            #visMenu.addAction( visAction )
            
            hideLocalAction.triggered.connect(viewIndexTarget.hideLocal)
            if not visible and not propagType:
              hideLocalAction.setChecked( True )

            hidePropagatedAction.triggered.connect(viewIndexTarget.hidePropagated)
            if not visible and propagType:
              hidePropagatedAction.setChecked( True )
            
            # THERE ARE BUGS WITH OVERRIDES
            #visAction = new QAction( "Hide (override)", 0 )
            #connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( hideOverride() ) )
            #visAction.setCheckable( True )
            #if( !visible and propagType == 2 )
            #  visAction.setChecked( True )
            #visMenu.addAction( visAction )
            
        
    elif len(selectedTreeItemIndexes) == 0 or len(selectedViewItemIndexes) == 0:
      assetMenu = SHAssetsMenu(self.shGLScene)
      lightsMenu = SHLightsMenu(self.shGLScene)
      self.addMenu(assetMenu)
      self.addMenu(lightsMenu)
