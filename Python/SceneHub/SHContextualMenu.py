
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI, Util
from FabricEngine.FabricUI import SceneHub
from FabricEngine.Util import *
from FabricEngine.SceneHub.SHBaseMenu import SHBaseMenu
from FabricEngine.SceneHub.SHLightsMenu import SHLightsMenu
from FabricEngine.SceneHub.SHAssetsMenu import SHAssetsMenu

class SHContextualMenu(SHBaseMenu):

  def __init__(self, client, scene, shStates, targetSGObject, treeView = None):
    self.targetSGObject = targetSGObject
    self.client = client

    self.treeView = treeView
    self.shStates = shStates
    super(SHContextualMenu, self).__init__(scene)
 
  def constructMenu(self):

    if self.targetSGObject is not None:
      expandAction = QtGui.QAction("Expand recursively", self)
      self.addAction(expandAction)
      loadAction = QtGui.QAction("Load recursively", self)
      self.addAction(loadAction)

      visMenu = self.addMenu("Visibility")

      showPropagatedAction = QtGui.QAction("Show", visMenu)
      showPropagatedAction.setCheckable( True )
      visMenu.addAction( showPropagatedAction )

      showLocalAction = QtGui.QAction("Show (local only)", visMenu)
      showLocalAction.setCheckable( True )
      visMenu.addAction( showLocalAction )

      hidePropagatedAction = QtGui.QAction("Hide", visMenu)
      hidePropagatedAction.setCheckable( True )
      visMenu.addAction( hidePropagatedAction )

      hideLocalAction = QtGui.QAction("Hide (local only)", visMenu)
      hideLocalAction.setCheckable( True )
      visMenu.addAction( hideLocalAction )

      resetVisibilityAction = QtGui.QAction("Reset recursively", visMenu)
      visMenu.addAction( resetVisibilityAction )

      if self.treeView is not None:
        for index in self.treeView.selectedIndexes():
          viewIndexTarget = SceneHub.SHTreeView_ViewIndexTarget(self.treeView, index, self)
          expandAction.triggered.connect(viewIndexTarget.expandRecursively)

      loadAction.triggered.connect(self.loadRecursively)

      visible = False
      propagVal = self.client.RT.types.UInt8(0)
      visible = self.targetSGObject.getVisibility("Boolean", propagVal).getSimpleType()
      propagType = propagVal.getSimpleType()
      
      showLocalAction.triggered.connect(self.showLocal)
      if visible and not propagType:
        showLocalAction.setChecked( True )

      showPropagatedAction.triggered.connect(self.showPropagated)
      if visible and propagType:
        showPropagatedAction.setChecked(True)

      # THERE ARE BUGS WITH OVERRIDES
      #visAction = new QAction( "Show (override)", 0 )
      #connect( visAction, SIGNAL( triggered() ), self, SLOT( showOverride() ) )
      #visAction.setCheckable( True )
      #if( visible and propagType == 2 )
      #  visAction.setChecked( True )
      #visMenu.addAction( visAction )
            
      hideLocalAction.triggered.connect(self.hideLocal)
      if not visible and not propagType:
        hideLocalAction.setChecked( True )

      hidePropagatedAction.triggered.connect(self.hidePropagated)
      if not visible and propagType:
        hidePropagatedAction.setChecked( True )
            
      # THERE ARE BUGS WITH OVERRIDES
      #visAction = new QAction( "Hide (override)", 0 )
      #connect( visAction, SIGNAL( triggered() ), self, SLOT( hideOverride() ) )
      #visAction.setCheckable( True )
      #if( !visible and propagType == 2 )
      #  visAction.setChecked( True )
      #visMenu.addAction( visAction )

      resetVisibilityAction.triggered.connect(self.resetVisibilityRecursively)
      
    else:
      assetMenu = SHAssetsMenu(self.shGLScene)
      lightsMenu = SHLightsMenu(self.shGLScene)
      self.addMenu(assetMenu)
      self.addMenu(lightsMenu)

  def loadRecursively(self):
    self.targetSGObject.forceHierarchyExpansion("")
    self.shStates.onStateChanged()

  def resetVisibilityRecursively(self):
    self.targetSGObject.resetVisibilityRecursively("")
    self.shStates.onStateChanged()

  def setVisibility(self, visible, propagationType):
    self.targetSGObject.setVisibility("", visible, propagationType)
    self.shStates.onStateChanged()

  def showLocal(self):
    self.setVisibility( True, 0 )
    
  def showPropagated(self):
    self.setVisibility( True, 1 )
    
  def showOverride(self):
    self.setVisibility( True, 2 )

  def hideLocal(self):
    self.setVisibility( False, 0 )
    
  def hidePropagated(self):
    self.setVisibility( False, 1 )
    
  def hideOverride(self):
    self.setVisibility( False, 2 )
