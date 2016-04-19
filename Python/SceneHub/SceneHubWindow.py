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

  def __init__(self, settings, unguarded, noopt, klFile):
    self.noopt = noopt
    self.klFile = klFile
    self.viewport = None
    self.shDFGBinding = None
    self.isCanvas = False
    super(SceneHubWindow, self).__init__(settings, unguarded)

  def _initKL(self, unguarded):
    super(SceneHubWindow, self)._initKL(unguarded)
    self.client.loadExtension('SceneHub')
    # Create the renderer
    self.viewportsManager = SHViewportsManager(self)
    self.shStates = SceneHub.SHStates(self.client)

  def _initDFG(self):
    super(SceneHubWindow, self)._initDFG()
    self.shDFGBinding = SceneHub.SHDFGBinding(self.mainBinding, self.dfgWidget.getUIController(), self.client)
  
  def _initTreeView(self):
    super(SceneHubWindow, self)._initTreeView()
    # Update the renderer in case it has been overriden by the main scene.
    self.shTreesManager = SHTreeViewsManager(self, self.klFile)
    self.viewportsManager.update()
    
    #JCG self.shTreesManager.shTreeView.itemSelected.connect(self.shDFGBinding.onTreeItemSelected)
    #JCG self.shTreesManager.sceneUpdated.connect(self.onSceneUpdated)
   
    self.shCmdHandler = SceneHub.SHCmdHandler(
      self.shTreesManager.getScene(), 
      SceneHub.SHCmdRegistration(),
      self.qUndoStack)

  def _initValueEditor(self):
    self.valueEditor = SceneHub.SHVEEditorOwner(self.dfgWidget, self.shTreesManager.shTreeView, None)

    self.valueEditor.log.connect(self._onLog)
    self.valueEditor.modelItemValueChanged.connect(self._onModelValueChanged)
    self.valueEditor.canvasSidePanelInspectRequested.connect(self._onCanvasSidePanelInspectRequested)
    
    self.shTreesManager.sceneHierarchyChanged.connect(self.valueEditor.onSceneChanged)
    self.shTreesManager.sceneUpdated.connect(self.valueEditor.onSceneChanged)

    #for( ViewportSet::iterator it = m_viewports.begin() it != m_viewports.end() ++it )
    #  QObject::connect( *it, SIGNAL( sceneChanged() ), m_valueEditor, SLOT( onSceneChanged() ) )

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
    dirtyList = self.shDFGBinding.setDirty()
    if not dirtyList[0]: super(SceneHubWindow, self).onDirty()
    if dirtyList[1]: self.viewportsManager.onRefreshAllViewports()

  def _onLog(self, message):
    if self.dfgWidget is not None:
      self.dfgWidget.getUIController().logError(message)
 
  def updateFPS(self):
    if not self.viewport: return 
    caption = str(float("{0:.2f}".format(self.viewport.fps())))
    caption += " FPS"
    
    # Viewport 0
    stats = self.viewportsManager.shGLRenderer.getDrawStats(0)
    caption += " Drawn obj: "+ str(stats[0])
    if stats[1] > 0: caption += " pt: "  + str(stats[1])
    if stats[2] > 0: caption += " li: "  + str(stats[2])
    if stats[3] > 0: caption += " tri: " + str(stats[3])
    
    self.fpsLabel.setText( caption )
    
  def _onShowUsage(self):
    helpWidget = HelpWidget(self)
    helpWidget.show()
