
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI, Util
from FabricEngine.FabricUI import SceneHub
from FabricEngine.Util import *
from FabricEngine.SceneHub.SHBaseSceneMenu import SHBaseSceneMenu
from FabricEngine.SceneHub.SHLightsMenu import SHLightsMenu
from FabricEngine.SceneHub.SHAssetsMenu import SHAssetsMenu

class SHContextualMenu(SHBaseSceneMenu):

    """SHContextualMenu

    SHContextualMenu specializes SHBaseSceneMenu and defines the contextual menu 
    used from either the treeView or the 3DView (viewports).
    It supports : Edit SGObject/SGObjectProperties if selected.
                  Add assets/ligths to the scene if no selection.

    Arguments:
        client (FabricEngine.Core.Client): A reference to the FabricCore.Client.
        shGLScene (SceneHub.SHGLScene): A reference to a SHGLScene.
        shStates (SceneHub.SHStates): A reference to the SHStates.
        targetSGObject (Fabr.RTVal): A reference to the sgObject to edit, pass as a RTVal.
        shTreeView (QWidget): A reference to the SceneHub.SHTreeView, can be None.
    """
    
    def __init__(self, client, shGLScene, shStates, targetSGObject, shTreeView = None):
        self.targetSGObject = targetSGObject
        self.client = client

        self.shTreeView = shTreeView
        self.shStates = shStates
        super(SHContextualMenu, self).__init__(shGLScene)
 
    def constructMenu(self):
        """Implementation of BaseMenu.
        """

        # If there is a selected object, either from the shTreeView or the 3View
        # acces/edits its properties.
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

            if self.shTreeView is not None:
                for index in self.shTreeView.selectedIndexes():
                    viewIndexTarget = SceneHub.SHshTreeView_ViewIndexTarget(self.shTreeView, index, self)
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
      
        # Otherwise, add archive/lights to the scene
        else:
            assetMenu = SHAssetsMenu(self.shGLScene)
            lightsMenu = SHLightsMenu(self.shGLScene)
            self.addMenu(assetMenu)
            self.addMenu(lightsMenu)

    def loadRecursively(self):
        """Loads recursively the object children.
        """

        self.targetSGObject.forceHierarchyExpansion("")
        self.shStates.onStateChanged()

    def resetVisibilityRecursively(self):
        """Reset recursively the object visibility.
        """
        
        self.targetSGObject.resetVisibilityRecursively("")
        self.shStates.onStateChanged()

    def setVisibility(self, visible, propagationType):
        """Sets the object visibility.
        """
        
        self.targetSGObject.setVisibility("", visible, propagationType)
        self.shStates.onStateChanged()

    def showLocal(self):
        """Shows the object.
        """
        
        self.setVisibility( True, 0 )

    def showPropagated(self):
        """Shows the object, propagates to it's instances.
        """

        self.setVisibility( True, 1 )

    def showOverride(self):
        """Shows the object parent override.
        """
        
        self.setVisibility( True, 2 )

    def hideLocal(self):
        """Hides the object.
        """
        
        self.setVisibility( False, 0 )

    def hidePropagated(self):
        """Hides the object, propagates to it's instances.
        """
        
        self.setVisibility( False, 1 )

    def hideOverride(self):
        """Hides the object parent override.
        """
        
        self.setVisibility( False, 2 )
