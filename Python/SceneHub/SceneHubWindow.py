import os, sys, math
from FabricEngine import Core, FabricUI, CAPI
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine.FabricUI import *
from FabricEngine.CAPI import *
from FabricEngine.Canvas.CanvasWindow import CanvasWindow
from FabricEngine.SceneHub.SHTreeViewsManager import SHTreeViewsManager
from FabricEngine.SceneHub.SHViewportsManager import SHViewportsManager
from FabricEngine.SceneHub.SHAssetsMenu import SHAssetsMenu
from FabricEngine.SceneHub.SHLightsMenu import SHLightsMenu
from FabricEngine.SceneHub.SHTreeViewMenu import SHTreeViewMenu
from FabricEngine.SceneHub.SHInteractionMenu import SHInteractionMenu
from FabricEngine.SceneHub.HelpWidget import HelpWidget
from FabricEngine.SceneHub.SHVEEditorOwner import SHVEEditorOwner


class SceneHubWindow(CanvasWindow):

  def __init__(self, settings, unguarded, noopt, klFile, canvasFile, samples):
    self.klFile = klFile
    self.viewport = None
    self.shDFGBinding = None
    self.isCanvas = False
    self.initSamples = samples
    super(SceneHubWindow, self).__init__(settings, unguarded, noopt)

    loadCanvas = canvasFile is not ""
    self._initApp(loadCanvas)
    if loadCanvas: self.loadGraph(canvasFile)

  def _initKL(self, unguarded, noopt):
    super(SceneHubWindow, self)._initKL(unguarded, noopt)
    self.client.loadExtension('SceneHub')
    # Create the renderer manager
    self.shStates = SceneHub.SHStates(self.client)
    self.viewportsManager = SHViewportsManager(self, self.shStates, self.initSamples)

  def _initDFG(self):
    super(SceneHubWindow, self)._initDFG()
    self.shDFGBinding = SceneHub.SHDFGBinding(
      self.mainBinding, 
      self.dfgWidget.getUIController(),
      self.shStates)
    self.shDFGBinding.sceneChanged.connect(self.shStates.onStateChanged)
    self.shStates.inspectedChanged.connect(self.onInspectChanged)

  def _initCommands(self):
    cmdRegistration = SceneHub.SHCmdRegistration()
    self.qUndoStack.indexChanged.connect(self.shStates.onStateChanged)

  def _initLog(self):
    super(SceneHubWindow, self)._initLog()
    self._initCommands()
    self.shCmdHandler = SceneHub.SHCmdHandler(self.client, self.qUndoStack)

  def _initTreeView(self):
    super(SceneHubWindow, self)._initTreeView()

    self.shTreesManager = SHTreeViewsManager(self.client, self.dfgWidget, self.shStates, self.klFile)
    self.shTreesManager.activeSceneChanged.connect( self.onActiveSceneChanged )

    # scene changed -> tree view changed
    self.shStates.sceneHierarchyChanged.connect(self.shTreesManager.onSceneHierarchyChanged)
    self.shStates.selectionChanged.connect(self.shTreesManager.onSelectionChanged)

    # tree view changed -> scene changed
    self.shTreesManager.sceneHierarchyChanged.connect(self.shStates.onStateChanged)
    self.shTreesManager.sceneChanged.connect(self.shStates.onStateChanged)

  def _initValueEditor(self):
    self.valueEditor = SHVEEditorOwner(self.dfgWidget, self.shStates)

    self.valueEditor.log.connect(self._onLog)
    self.valueEditor.modelItemValueChanged.connect(self._onModelValueChanged)
    self.valueEditor.canvasSidePanelInspectRequested.connect(self._onCanvasSidePanelInspectRequested)
    self.valueEditor.synchronizeCommands.connect(self.shCmdHandler.onSynchronizeCommands)

    self.shStates.inspectedChanged.connect(self.valueEditor.onInspectChanged)
    self.shStates.activeSceneChanged.connect(self.valueEditor.onSceneChanged)
    self.shStates.sceneChanged.connect(self.valueEditor.onSceneChanged)
  
  def _initGL(self):
    self.viewport, intermediateOwnerWidget = self.viewportsManager.createViewport(
      0, 
      False, 
      False, 
      None)
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
    self.assetMenu = SHAssetsMenu(self.shStates.getActiveScene())
    self.lightsMenu = SHLightsMenu(self.shStates.getActiveScene())
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

    # This has to be done after self.valueEditor.initConnections
    if self.shStates.getActiveScene().showValueEditorByDefault():
      self.valueEditor.updateSGObject(self.shStates.getActiveScene().getValueEditorDefaultTarget())
      self.valueEditorDockWidget.show();

    defaultCanvasOp = self.shStates.getActiveScene().getDefaultSGCanvasOperator()
    if defaultCanvasOp is not None:
      self.dfgDock.show()
      self.shDFGBinding.setCanvasOperator(defaultCanvasOp)
    
    if self.shTreesManager.getScene().enableTimelineByDefault():
      startFrame, endFrame = self.shTreesManager.getScene().getFrameState()
      self.timeLine.setTimeRange(startFrame, endFrame)
      self.timeLine.setFrameRate(self.shTreesManager.getScene().getFPS())
      self.timeLineDock.show()
    else: selftimeLineDock.hide()
    
    if self.shTreesManager.getScene().playbackByDefault(): 
      self._onTogglePlayback()

    self.adjustSize()

  def sizeHint(self):
    width = self.size().width()
    height = self.size().height()
    if width < 300: width = 300
    if height < 300: height = 300
    return QtCore.QSize(width, height)  

  def loadGraph(self, filePath):
    super(SceneHubWindow, self).loadGraph(filePath)
    self.shDFGBinding.setMainBinding(self.dfgWidget.getDFGController().getBinding())
    
  def onNewGraph(self, skip_save=False):
    super(SceneHubWindow, self).onNewGraph(skip_save)
    self.shDFGBinding.setMainBinding(self.dfgWidget.getDFGController().getBinding())

  def _contentChanged(self) :
    self.valueEditor.onOutputsChanged()
    self.viewportsManager.onRefreshAllViewports()

  def _onTogglePlayback(self):  
    if self.timeLineDock.isVisible() == False : self.timeLineDock.show()
    self.timeLine.play()
   
  def _onCanvasSidePanelInspectRequested(self):
    if self.shDFGBinding is not None:
      parameterObject = self.shDFGBinding.getCanvasOperatorParameterObject()
      if parameterObject is not None: 
        self.valueEditor.updateSGObject(parameterObject)
  
  def _onModelValueChanged(self, item, var):
    self.viewportsManager.onRefreshAllViewports()

  def onActiveSceneChanged(self, scene):
    self.assetMenu.onActiveSceneChanged(scene)
    self.lightsMenu.onActiveSceneChanged(scene)

  def onInspectChanged(self):
    # shDFGBinding might change the active binding
    self.shDFGBinding.onInspectChanged()
    binding = self.dfgWidget.getDFGController().getBinding()
    self.scriptEditor.updateBinding(binding)

  def onFrameChanged(self, frame):
    super(SceneHubWindow, self).onFrameChanged(frame)
    self.shStates.onFrameChanged(frame)

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
