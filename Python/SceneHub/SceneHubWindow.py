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

  def __init__(self, settings, unguarded, noopt, klFile, canvasFile):
    self.klFile = klFile
    self.viewport = None
    self.shDFGBinding = None
    self.isCanvas = False

    super(SceneHubWindow, self).__init__(settings, unguarded, noopt)

    isCanvas = canvasFile is not ""
    self._initApp(isCanvas)
    if isCanvas: self.loadGraph(canvasFile)

  def _initKL(self, unguarded, noopt):
    super(SceneHubWindow, self)._initKL(unguarded, noopt)
    self.client.loadExtension('SceneHub')
    # Create the renderer
    self.shStates = SceneHub.SHStates(self.client)
    self.viewportsManager = SHViewportsManager(self, self.shStates)

  def _initDFG(self):
    super(SceneHubWindow, self)._initDFG()
    self.shDFGBinding = SceneHub.SHDFGBinding(self.mainBinding, self.dfgWidget.getUIController(), self.client)
    self.shDFGBinding.sceneChanged.connect(self.shStates.onStateChanged)
  
  def _initCommands(self):
    cmdRegistration = SceneHub.SHCmdRegistration()
    self.qUndoStack.indexChanged.connect(self.shStates.onStateChanged)

  def _initLog(self):
    super(SceneHubWindow, self)._initLog()
    self._initCommands()
    self.shCmdHandler = SceneHub.SHCmdHandler(
      self.client, 
      self.qUndoStack)

  def _initTreeView(self):
    super(SceneHubWindow, self)._initTreeView()
    # Update the renderer in case it has been overriden by the main scene.
    self.shTreesManager = SHTreeViewsManager(self, self.shStates, self.klFile)
    
    #JCG self.shTreesManager.shTreeView.itemSelected.connect(self.shDFGBinding.onTreeItemSelected)
    self.shStates.sceneHierarchyChanged.connect(self.shTreesManager.onSceneHierarchyChanged)
    self.shStates.selectionChanged.connect(self.shTreesManager.onSelectionChanged)

  def _initValueEditor(self):
    self.valueEditor = SceneHub.SHVEEditorOwner(self.dfgWidget, self.shTreesManager.shTreeView)

    self.valueEditor.log.connect(self._onLog)
    self.valueEditor.modelItemValueChanged.connect(self._onModelValueChanged)
    self.valueEditor.canvasSidePanelInspectRequested.connect(self._onCanvasSidePanelInspectRequested)
    self.valueEditor.synchronizeCommands.connect(self.shCmdHandler.onSynchronizeCommands)

    self.shStates.activeSceneChanged.connect(self.valueEditor.onSceneChanged)
    self.shStates.sceneChanged.connect(self.valueEditor.onSceneChanged)

    #self.shTreesManager.sceneHierarchyChanged.connect(self.valueEditor.onSceneChanged)
    #self.shTreesManager.activeSceneChanged.connect(self.valueEditor.onSceneChanged)
    #self.shDFGBinding.sceneChanged.connect(self.valueEditor.onSceneChanged)

  def _initGL(self):
    self.viewport, intermediateOwnerWidget = self.viewportsManager.createViewport(0, False, False, None)
    self.setCentralWidget(intermediateOwnerWidget)
    self.viewport.makeCurrent()  

  def _initTimeLine(self):
    super(SceneHubWindow, self)._initTimeLine()
    self.timeLine.playbackChanged.connect(self.viewportsManager.onPlaybackChanged)
    self.timeLine.setTimeRange(0, 100)
    self.timeLine.setFrameRate(24)
  
  def _initDocks(self):
    super(SceneHubWindow, self)._initDocks()
    self.shTreeDock = QtGui.QDockWidget("Tree-View", self)
    self.shTreeDock.setObjectName("Tree-View")
    self.shTreeDock.setFeatures(self.dockFeatures)
    self.shTreeDock.setWidget(self.shTreesManager)
    self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.shTreeDock, QtCore.Qt.Vertical)

  def _initMenus(self):
    super(SceneHubWindow, self)._initMenus()

    self.treeViewMenu = SHTreeViewMenu(self.shTreesManager)
    menus = self.menuBar().findChildren(QtGui.QMenu)
    for menu in menus:      
      if menu.title() == "&Window":
        toggleAction = self.shTreeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_8)
        menu.addAction(toggleAction)
        menu.addSeparator()
      if menu.title() == "&View":
        menu.addSeparator()
        menu.addMenu(self.treeViewMenu)

    self.viewportsManager.initMenu(self.menuBar())

    sceneMenus = self.menuBar().addMenu("&Scene")    
    self.assetMenu = SHAssetsMenu(self.shStates)
    self.lightsMenu = SHLightsMenu(self.shStates)
    sceneMenus.addMenu(self.assetMenu)
    sceneMenus.addMenu(self.lightsMenu)

    self.interactionMenu = self.menuBar().addMenu("&Interaction")
    togglePlaybackAction = self.interactionMenu.addAction("Toggle Playback")
    togglePlaybackAction.triggered.connect(self._onTogglePlayback)
    self.interactionMenu.addMenu(SHInteractionMenu(self.viewportsManager.shGLRenderer))

    helpMenu = self.menuBar().addMenu("&Help")
    usageAction = helpMenu.addAction("Show Usage")
    usageAction.triggered.connect(self._onShowUsage)

  def _initApp(self, isCanvas):
    show, level = self.shTreesManager.getScene().showTreeViewByDefault()
    if show:
      self.shTreesManager.expandTree(level)
      self.shTreeDock.show()
    
    else: self.shTreeDock.hide()
   
    if not isCanvas:
      self.treeDock.hide()
      self.logDockWidget.hide()
      self.valueEditorDockWidget.hide()
      self.dfgDock.hide()
      self.scriptEditorDock.hide()

    if self.shTreesManager.getScene().enableTimelineByDefault():
      startFrame, endFrame = self.shTreesManager.getScene().getFrameState()
      self.timeLine.setTimeRange(startFrame, endFrame)
      self.timeLine.setFrameRate(self.shTreesManager.getScene().getFPS())
      self.timeLineDock.show()
    else: selftimeLineDock.hide()
    
    if self.shTreesManager.getScene().playbackByDefault(): self._onTogglePlayback()

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
    self.shStates.onFrameChanged(frame)
    #JCG self.shTreesManager.getScene().setFrame(frame)
    #JCG self.viewportsManager.onRefreshAllViewports()

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
