import os, sys, math
from FabricEngine import Core, FabricUI
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine.FabricUI import *
from Canvas.CanvasWindow import CanvasWindow
from SHTreeViewsManager import SHTreeViewsManager
from SHViewportsManager import SHViewportsManager
from SHAssetsMenu import SHAssetsMenu
from SHLightsMenu import SHLightsMenu
from SHTreeViewMenu import SHTreeViewMenu
from SHInteractionMenu import SHInteractionMenu
from HelpWidget import HelpWidget

class SceneHubWindow(CanvasWindow):

  def __init__(self, fabricDir, settings, unguarded, noopt, klFile):
    self.noopt = noopt
    self.klFile = klFile
    self.viewport = None
    self.shDFGBinding = None
    self.isCanvas = False
    super(SceneHubWindow, self).__init__(fabricDir, settings, unguarded)

  def _initKL(self, unguarded):
    super(SceneHubWindow, self)._initKL(unguarded)
    self.client.loadExtension('SceneHub')
    # Create the renderer
    self.viewportsManager = SHViewportsManager(self)
    self.shTreesManager = SHTreeViewsManager(self, self.klFile)

  def _intDFG(self):
    super(SceneHubWindow, self)._intDFG()
    self.shDFGBinding = SceneHub.SHDFGCanvas(self.mainBinding, self.dfgWidget.getUIController(), self.client)
    self.shTreesManager.shTreeView.itemSelected.connect(self.shDFGBinding.onTreeItemSelected)

  def _initTreeView(self):
    super(SceneHubWindow, self)._initTreeView()
    # Update the renderer in case it has been overriden by the main scene.
    self.viewportsManager.update()
    self.shTreesManager.sceneUpdated.connect(self.onSceneUpdated)

    self.shCmdRegistration = SceneHub.SHCmdRegistration()

    self.shCmdHandler = SceneHub.SHCmdHandler(
      self.shTreesManager.getScene(), 
      self.shCmdRegistration,
      self.qUndoStack)
 
  def _initValueEditor(self):
    self.valueEditor = SceneHub.SHVEEditorOwner(self.dfgWidget, self.shTreesManager.shTreeView, None)
    if self.shTreesManager is not None:
      self.shTreesManager.sceneHierarchyChanged.connect(self.valueEditor.onSceneChanged)
      self.shTreesManager.sceneUpdated.connect(self.valueEditor.onSceneChanged)
    
    #for( ViewportSet::iterator it = m_viewports.begin() it != m_viewports.end() ++it )
    #  QObject::connect( *it, SIGNAL( sceneChanged() ), m_valueEditor, SLOT( onSceneChanged() ) )

    self.valueEditor.log.connect(self._onLog)
    self.valueEditor.modelItemValueChanged.connect(self._onModelValueChanged)
    self.valueEditor.canvasSidePanelInspectRequested.connect(self._onCanvasSidePanelInspectRequested)

  def _initGL(self):
    self.viewport, intermediateOwnerWidget = self.viewportsManager.createViewport(0, False, False, None)
    self.setCentralWidget(intermediateOwnerWidget)
    self.viewport.makeCurrent()  

  def _initDocksAndMenus(self):
    super(SceneHubWindow, self)._initDocksAndMenus()
    shTreeDock = QtGui.QDockWidget("Tree-View", self)
    shTreeDock.setObjectName("Tree-View")
    shTreeDock.setFeatures(self.dockFeatures)
    shTreeDock.setWidget(self.shTreesManager)
    self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, shTreeDock, QtCore.Qt.Vertical)

    self.treeViewMenu = SHTreeViewMenu(self.shTreesManager)
    menus = self.menuBar().findChildren(QtGui.QMenu)
    for menu in menus:      
      if menu.title() == "&Window":
        toggleAction = shTreeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_8)
        menu.addAction(toggleAction)
        menu.addSeparator()
      if menu.title() == "&View":
        menu.addSeparator()
        menu.addMenu(self.treeViewMenu)

    self.viewportsManager.initMenu(self.menuBar())

    sceneMenus = self.menuBar().addMenu("&Scene")    
    self.assetMenu = SHAssetsMenu(self.shTreesManager.getScene())
    self.lightsMenu = SHLightsMenu(self.shTreesManager.getScene())
    sceneMenus.addMenu(self.assetMenu)
    sceneMenus.addMenu(self.lightsMenu)

    self.interactionMenu = self.menuBar().addMenu("&Interaction")
    togglePlaybackAction = self.interactionMenu.addAction("Toggle Playback")
    togglePlaybackAction.triggered.connect(self._onTogglePlayback)
    self.interactionMenu.addMenu(SHInteractionMenu(self.viewportsManager.shGLRenderer))


    helpMenu = self.menuBar().addMenu("&Help")
    usageAction = helpMenu.addAction("Show Usage")
    usageAction.triggered.connect(self._onShowUsage)

  def _contentChanged(self) :
    self.valueEditor.onOutputsChanged()
    self.viewportsManager.onRefreshAllViewports()

  def _onTogglePlayback(self):  
    if self.timeLineDock.isVisible() == False : self.timeLineDock.show()
    self.timeLine.play()
   
  def _onCanvasSidePanelInspectRequested(self):
    if self.shDFGBinding is not None and self.shDFGBinding.isSgObjectValid(): 
      self.valueEditor.updateSGObject(self.shDFGBinding)
  
  def _onModelValueChanged(self, item, var):
    self.viewportsManager.onRefreshAllViewports()

  def onFrameChanged(self, frame):
    super(SceneHubWindow, self).onFrameChanged(frame)
    self.shTreesManager.getScene().setFrame(frame)
    self.viewportsManager.onRefreshAllViewports()

  def onSceneUpdated(self, scene):
    self.viewportsManager.onSceneUpdated(scene)
    self.assetMenu.onSceneUpdated(scene)
    self.lightsMenu.onSceneUpdated(scene)
    self.shCmdHandler.onSceneUpdated(scene)

  def onDirty(self):
    if self.shDFGBinding is not None:
      accepted = bool()
      refresh = bool()
      m_shDFGBinding.setDirty(accepted, refresh)
      if not accepted: super(SceneHubWindow, self).onDirty()
      if refresh: self.viewportsManager.onRefreshAllViewports()

  def _onLog(self, message):
    if self.dfgWidget is not None:
      self.dfgWidget.getUIController().logError(message)
 
  def updateFPS(self):
    pass
    '''
    #super(SceneHubWindow, self).updateFPS()
    #FabricCore::RTVal scene = m_shMainGLScene->getSHGLScene()
    if ( !self.viewport ) return # || !scene.isValid() ) return
    caption = str(self.viewport.fps())
    caption += " FPS"
    
    obj = int point, line, triangle
    # Viewport 0
    stats = m_shGLRenderer.getDrawStats(0)

    std::ostringstream ss
    ss << " Drawn obj: " << stats[0]
    if( point ) ss << " pt: " << stats[1]
    if( line ) ss << " li: " << stats[2]
    if( triangle ) ss << " tri: " << stats[3]
    caption += ss.str().c_str()
   
    self.fpsLabel.setText( caption )
    '''
  
  def _onShowUsage(self):
    helpWidget = HelpWidget(self)
    helpWidget.show()
