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
#from FabricEngine.SceneHub.SHVEEditorOwner import SHVEEditorOwner
from FabricEngine.SceneHub.SHHelpWidget import SHHelpWidget

class SceneHubWindow(CanvasWindow):

    """SceneHubWindow

    This window encompasses the entire SceneHub application.

    Arguments:
        settings (QtCore.QSettings): Settings object that is used to store and
            retrieve settings for the application.
        unguarded (bool): Whether to create the Fabric client in unguarded mode.
        noopt (bool): Whether to create the Fabric client in noopt mode.
        klFile (str): Path of a SceneHub scene to load on stratup.
        canvasFile (str): Path of a Canvas graph to load on stratup.
        samples (int): Number of multi-sampling on startup.
        usageFilePath (str): The path of the application usage file.

    """


    def __init__(
        self, 
        settings, 
        unguarded, 
        noopt, 
        klFile, 
        canvasFile, 
        samples, 
        usageFilePath = None):

        self.klFile = klFile
        self.viewport = None
        self.shDFGBinding = None
        self.isCanvas = False
        self.initSamples = samples
        self.usageFilePath = usageFilePath
        super(SceneHubWindow, self).__init__(settings, unguarded, noopt)

        loadCanvas = canvasFile is not ""
        self._initApp(loadCanvas)
        if loadCanvas: self.loadGraph(canvasFile)

    def _initKL(self, unguarded, noopt):
        """ Implementation of Canvas.CanvasWindow.
        """

        super(SceneHubWindow, self)._initKL(unguarded, noopt)
        self.client.loadExtension('SceneHub')
        # Create the main scene and states
        self.shStates = SceneHub.SHStates(self.client)
        self.shMainGLScene = SceneHub.SHGLScene(self.client, self.klFile)

        # The scene might create a specialized renderer type; get it from the scene
        shRenderer = self.shMainGLScene.getSHGLRenderer()

        # Create the renderer manager
        self.viewportsManager = SHViewportsManager(self, self.shStates, shRenderer, self.initSamples)

    def _initDFG(self):
        """ Implementation of Canvas.CanvasWindow.
        """

        super(SceneHubWindow, self)._initDFG()
        self.shDFGBinding = SceneHub.SHDFGBinding(
            self.mainBinding, 
            self.dfgWidget.getUIController(),
            self.shStates)
        self.shDFGBinding.sceneChanged.connect(self.shStates.onStateChanged)
        self.shStates.inspectedChanged.connect(self.onInspectChanged)

    def _initLog(self):
        """ Implementation of Canvas.CanvasWindow.
        """

        super(SceneHubWindow, self)._initLog()
        self.qUndoStack.indexChanged.connect(self.shStates.onStateChanged)
        self.shCmdHandler = SceneHub.SHCmdHandler(self.client, self.qUndoStack)
        self.shCmdHandler.registerCommands()

    def _initTreeView(self):
        """ Implementation of Canvas.CanvasWindow.
        """

        super(SceneHubWindow, self)._initTreeView()

        self.shTreesManager = SHTreeViewsManager(self.client, self.dfgWidget, self.shStates, self.shMainGLScene)
        self.shTreesManager.activeSceneChanged.connect( self.onActiveSceneChanged )

        # scene changed -> tree view changed
        self.blockedTreeViewHierarchyChanged = False
        self.shStates.sceneHierarchyChanged.connect(self.onSceneHierarchyChanged)
        #self.shStates.sceneHierarchyChanged.connect(self.shTreesManager.onSceneHierarchyChanged)
        self.shStates.selectionChanged.connect(self.shTreesManager.onSelectionChanged)

        # tree view changed -> scene changed
        self.shTreesManager.sceneHierarchyChanged.connect(self.shStates.onStateChanged)
        self.shTreesManager.sceneChanged.connect(self.shStates.onStateChanged)

    def _initValueEditor(self):
        """ Override of Canvas.CanvasWindow.
        """

        self.valueEditor = SceneHub.SHVEEditorOwner(self.dfgWidget, self.shStates)

        self.valueEditor.log.connect(self._onLog)
        self.valueEditor.modelItemValueChanged.connect(self._onModelValueChanged)
        self.valueEditor.canvasSidePanelInspectRequested.connect(self._onCanvasSidePanelInspectRequested)
        self.valueEditor.synchronizeCommands.connect(self.shCmdHandler.onSynchronizeCommands)

        self.shStates.inspectedChanged.connect(self.valueEditor.onInspectChanged)
        self.shStates.activeSceneChanged.connect(self.valueEditor.onSceneChanged)
        self.shStates.sceneChanged.connect(self.valueEditor.onSceneChanged)
  
    def _initGL(self):
        """ Override of Canvas.CanvasWindow.
        """

        # Create the first viewports
        self.viewport, intermediateOwnerWidget = self.viewportsManager.createViewport(
            0, 
            False, 
            False, 
            None)
        self.setCentralWidget(intermediateOwnerWidget)
        self.viewport.makeCurrent()  

    def _initTimeLine(self):
        """ Implementation of Canvas.CanvasWindow.
        """

        super(SceneHubWindow, self)._initTimeLine()
        self.timeLine.playbackChanged.connect(self.viewportsManager.onPlaybackChanged)
        self.timeLine.setTimeRange(0, 100)
        self.timeLine.setFrameRate(24)
      
    def _initDocks(self):
        """ Implementation of Canvas.CanvasWindow.
        """

        super(SceneHubWindow, self)._initDocks()
        self.shTreeDock = QtGui.QDockWidget("Tree-View", self)
        self.shTreeDock.setObjectName("Tree-View")
        self.shTreeDock.setFeatures(self.dockFeatures)
        self.shTreeDock.setWidget(self.shTreesManager)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.shTreeDock, QtCore.Qt.Vertical)

    def _initMenus(self):
        """ Final initilization of the app, just before running.
        """

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

        selectionMenu = self.interactionMenu.addMenu("Selection")
        self.toggleSharedObjectSelection = selectionMenu.addAction("Allow shared scene object selection")
        self.toggleSharedObjectSelection.setCheckable(True)
        self.toggleSharedObjectSelection.triggered.connect(self._onToggleSharedObjectSelection)

        helpMenu = self.menuBar().addMenu("&Help")
        usageAction = helpMenu.addAction("Show Usage")
        usageAction.triggered.connect(self._onShowUsage)

    def _initApp(self, isCanvas):
        """ Final initilization of the app, just before running.
        """

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
            binding = self.dfgWidget.getDFGController().getBinding()
            self.scriptEditor.updateBinding(binding)
        
        if self.shTreesManager.getScene().enableTimelineByDefault():
            startFrame, endFrame = self.shTreesManager.getScene().getFrameState()
            self.timeLine.setTimeRange(startFrame, endFrame)
            self.timeLine.setFrameRate(self.shTreesManager.getScene().getFPS())
            self.timeLineDock.show()
        else: 
            self.timeLineDock.hide()
        
        if self.shTreesManager.getScene().playbackByDefault(): 
            self._onTogglePlayback()

        if self.shTreesManager.getScene().refreshAlways(): 
            self._onRefreshAlways()

        self.adjustSize()

        # Create a timer to refresh in case there are asynchronous tasks (eg: background loading)
        self.checkAsyncSceneStateTimer = QtCore.QTimer( self )
        self.checkAsyncSceneStateTimer.setInterval( 333 ) # 3 times per second
        self.checkAsyncSceneStateTimer.timeout.connect(self.checkAsyncState)
        self.checkAsyncSceneStateTimer.start()
        
    def sizeHint(self):
        """ Returns the default windows size on startup.
        """

        width = self.size().width()
        height = self.size().height()
        if width < 300: width = 300
        if height < 300: height = 300
        return QtCore.QSize(width, height)  

    def loadGraph(self, filePath):
        """ Implementation of Canvas.CanvasWindow.
        """

        super(SceneHubWindow, self).loadGraph(filePath)
        self.shDFGBinding.setMainBinding(self.dfgWidget.getDFGController().getBinding())
    
    def onNewGraph(self, skip_save=False):
        """ Implementation of Canvas.CanvasWindow.
        """

        super(SceneHubWindow, self).onNewGraph(skip_save)
        self.shDFGBinding.setMainBinding(self.dfgWidget.getDFGController().getBinding())

    def _contentChanged(self):
        """ Implementation of Canvas.CanvasWindow.
        """

        self.valueEditor.onOutputsChanged()
        self.viewportsManager.onRefreshAllViewports()

    def _onTogglePlayback(self): 
        """ Toggles the playback.
        """

        if self.timeLineDock.isVisible() == False: 
            self.timeLineDock.hide()
        self.timeLine.play()

    def _onToggleSharedObjectSelection(self):
        self.viewportsManager.getSHRenderer().enableSharedObjectSelection( self.toggleSharedObjectSelection.isChecked() )

    def _onRefreshAlways(self): 
        """ Notifies all viewports that they should always refresh
        """

        self.timeLineDock.hide()
        self.timeLine.pause()

        self.viewportsManager.onAlwaysRefresh()
       
    def _onCanvasSidePanelInspectRequested(self):
        """ Refreshs the valueEditor from DFGBinding.
        """

        if self.shDFGBinding is not None:
            parameterObject = self.shDFGBinding.getCanvasOperatorParameterObject()
            if parameterObject is not None: 
                self.valueEditor.updateSGObject(parameterObject)
  
    def _onModelValueChanged(self, item, var):
        """ Refreshs the render when the valueEditor changed.
        """

        self.viewportsManager.onRefreshAllViewports()

    def onActiveSceneChanged(self, scene):
        """ Updates when the active scene changed from the treeViewManager.
        """

        self.assetMenu.onActiveSceneChanged(scene)
        self.lightsMenu.onActiveSceneChanged(scene)

    def onInspectChanged(self):
        """ Updates the valueEditor object/property to edit.
        """

        # shDFGBinding might change the active binding
        self.shDFGBinding.onInspectChanged()
        binding = self.dfgWidget.getDFGController().getBinding()
        self.scriptEditor.updateBinding(binding)

    def onFrameChanged(self, frame):
        """ Implementation of Canvas.CanvasWindow.
        """

        super(SceneHubWindow, self).onFrameChanged(frame)
        self.shStates.onFrameChanged(frame)

    def onDirty(self):
        """ Implementation of Canvas.CanvasWindow.
        """

        dirtyList = self.shDFGBinding.setDirty()
        if not dirtyList[0]: 
            super(SceneHubWindow, self).onDirty()
        if dirtyList[1]: 
            self.viewportsManager.onRefreshAllViewports()

    def _onLog(self, message):
        """ Override of Canvas.CanvasWindow.
        """

        if self.dfgWidget is not None:
            self.dfgWidget.getUIController().logError(message)
         
    def updateFPS(self):
        """ Override of Canvas.CanvasWindow.
        """

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
        """ Displays the application usage.
        """

        if self.usageFilePath is not None:
            dialog = QtGui.QDialog(self)
            helpWidget = SHHelpWidget(self.usageFilePath, dialog)
            helpWidget.closeSignal.connect(dialog.close)
            dialog.show()


    def onSceneHierarchyChanged(self):
        if self.viewportsManager.viewportUpdateRequested():
            self.blockedTreeViewHierarchyChanged = True
        elif not self.blockedTreeViewHierarchyChanged:
            self.shTreesManager.onSceneHierarchyChanged()

    def checkAsyncState(self):
        """ Requests a scene state change check in case it asynchronously changed,
            but only if there is not a prending refresh already.
        """

        if not self.viewportsManager.viewportUpdateRequested():
            self.shStates.onStateChanged()
            if self.blockedTreeViewHierarchyChanged:
                self.blockedTreeViewHierarchyChanged = False
                self.shTreesManager.onSceneHierarchyChanged()

