import optparse, os, sys, math, copy
from FabricEngine import Core, FabricUI
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine.FabricUI import *
from CanvasWindow import CanvasWindow
#from SHTreeViewWidget import SHTreeViewWidget

class SceneHubWindow(CanvasWindow):

  def __init__(
    self, 
    settings, 
    unguarded, 
    noopt, 
    klFile, 
    qglContext):
    self.nextViewportIndex = 1
    self.qglContext = [] 
    self.viewports = []
    self.sampleActions = []
    self.isCanvas = False
    self.viewport = None
    self.klFile = klFile
    self.noopt = noopt
    super(SceneHubWindow, self).__init__(settings, unguarded)

  def _initKL(self, unguarded):
    super(SceneHubWindow, self)._initKL(unguarded)
    self.client.loadExtension('Manipulation')
    self.client.loadExtension('SceneHub')
    # Create the renderer, only one.
    self.shGLRenderer = SceneHub.SHGLRenderer(self.client)
    # Construct the "main scene"
    self.shMainGLScene = SceneHub.SHGLScene(self.client, self.klFile)
    # Update the renderer in case it has been overriden by the main scene.
    self.shGLRenderer.update()

  def _initGL(self):
    self.samples = 1#self.qglContext[0].format().samples()
    self.viewport, intermediateOwnerWidget = self.__createViewport(0, False, None)
    self.setCentralWidget(intermediateOwnerWidget)
    self.viewport.makeCurrent()

  def _initTreeView(self):
    super(SceneHubWindow, self)._initTreeView()
    self.shTreeViewWidget = SceneHub.SHTreeViewWidget(self.shMainGLScene, self.dfgWidget.getUIController())
 
  def __updateSampleChecks(self):
    self.sampleActions[0].setChecked(False)
    self.sampleActions[1].setChecked(False)
    self.sampleActions[2].setChecked(False)
    self.sampleActions[3].setChecked(False)
    if( self.samples == 1 ):   self.sampleActions[0].setChecked(True)
    elif( self.samples == 2 ): self.sampleActions[1].setChecked(True)
    elif( self.samples == 4 ): self.sampleActions[2].setChecked(True)
    elif( self.samples == 8 ): self.sampleActions[3].setChecked(True)

  def _initDocksAndMenus(self):
    super(SceneHubWindow, self)._initDocksAndMenus()
    shTreeDock = QtGui.QDockWidget("Sh Tree-View", self)
    shTreeDock.setObjectName("SH Tree-View")
    shTreeDock.setFeatures(self.dockFeatures)
    shTreeDock.setWidget(self.shTreeViewWidget)
    self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, shTreeDock, QtCore.Qt.Vertical)

    menus = self.menuBar().findChildren(QtGui.QMenu)
    for menu in menus:
      if menu.title() == "&Window":
        toggleAction = shTreeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_8)
        menu.addAction(toggleAction)

        menu.addSeparator();
        viewportMenu = menu.addMenu("Add Viewport")
        viewportAction = viewportMenu.addAction("Add Perspective Viewport")
        viewportAction.triggered.connect(self.onAddViewport)
        viewportAction = viewportMenu.addAction("Add Orthographic Viewport")
        viewportAction.triggered.connect(self.onAddOrthoViewport)

        multisampleMenu = menu.addMenu("Multisampling")
        for i in range(0, 4):
          self.sampleActions.append(multisampleMenu.addAction(str(int(math.pow(2, i)))))
          self.sampleActions[i].setCheckable(True)
          self.sampleActions[i].triggered.connect(self.onSetSamples)
        self.__updateSampleChecks()
      
      if menu.title() == "&File":
        actions = menu.findChildren(QtGui.QAction)
        for action in actions:
          if action.text() == "Quit":
            importAssetAction = QtGui.QAction( "Import Alembic/Fbx", menu );
            importAssetAction.triggered.connect(self.onImportAsset);
            menu.insertAction( action, importAssetAction );

            exportToAlembicAction = QtGui.QAction( "Export to Alembic", menu );
            exportToAlembicAction.triggered.connect(self.onExportToAlembic);
            menu.insertAction( action, exportToAlembicAction );
            menu.insertSeparator( action );


  def _contentChanged(self) :
    self.valueEditor.onOutputsChanged()
    self.onRefreshAllViewports()

  def __createViewport(self, viewportIndex, orthographic, widgetToReplace):

    # If the multisampling has changed, we need to replace the RTRGLViewportWidget with a new one, since there
    # is a 1-to-1 correspondance between the QGlWidget, its context, and context's sampling options.
    # To do it, we add an intermediate widget, otherwise the layout collapses when one widget with another one.
    intermediateLayout = None
    intermediateOwnerWidget = None
 
    if not widgetToReplace:
      # initializing
      intermediateOwnerWidget = QtGui.QWidget(self)
      intermediateLayout = QtGui.QStackedLayout()
      intermediateOwnerWidget.setLayout(intermediateLayout)
       
      format = QtOpenGL.QGLFormat()
      format.setSamples(self.samples)
      format.setSampleBuffers(self.samples > 1)
      self.qglContext.append(Viewports.RTRGLContext( format ))

    else:
      intermediateOwnerWidget = widgetToReplace.parent()
      intermediateLayout = intermediateOwnerWidget.layout()
      # We will recreate the RTR viewport associated with the index
      widgetToReplace.detachFromRTRViewport()
      # create a context from the previous one, but change samples
      format = widgetToReplace.context().format()
      format.setSamples(self.samples)
      format.setSampleBuffers(self.samples > 1)
      self.qglContext[viewportIndex] = Viewports.RTRGLContext(format)

    temp = None
    if widgetToReplace is not None: temp = widgetToReplace
    else: temp = self.viewport

    newViewport = Viewports.RTRGLViewportWidget(
      self.client,
      self.shGLRenderer,
      self.shTreeViewWidget.getScene(),
      viewportIndex, 
      self.qglContext[viewportIndex], 
      self, 
      temp, 
      self.settings)
 
    if orthographic is not None:
      newViewport.setOrthographic(True)

    intermediateLayout.addWidget(newViewport)
    if widgetToReplace is not None:
      index = self.viewports.index(widgetToReplace)
      intermediateLayout.removeWidget(widgetToReplace)
      self.viewports.remove(widgetToReplace)
      widgetToReplace.deleteLater()
    self.viewports.append(newViewport)

    self.shTreeViewWidget.sceneHierarchyChanged.connect(self.onRefreshAllViewports)
    self.shTreeViewWidget.sceneUpdated.connect(newViewport.onSceneUpdated)
    newViewport.sceneChanged.connect(self.shTreeViewWidget.onSceneHierarchyChanged)
    newViewport.viewportDestroying.connect(self.onViewportDestroying)
    newViewport.sceneChanged.connect(self.onRefreshAllViewports)
    newViewport.manipsAcceptedEvent.connect(self.shTreeViewWidget.updateFrom3DSelection)
    newViewport.manipsAcceptedEvent.connect(self.onRefreshAllViewports)

    #if(self.shCmdViewWidget)
    #  newViewport.synchronizeCommands.connect( self.shCmdViewWidget.synchronize() )

    return newViewport, intermediateOwnerWidget;

  def __addViewport(self, orthographic):
    index = self.nextViewportIndex
    self.nextViewportIndex = self.nextViewportIndex + 1
    print "self.nextViewportIndex " + str(self.nextViewportIndex)
    _, intermediateOwnerWidget = self.__createViewport(index, orthographic, None)
    
    name = "Viewport " + str(index)
    viewportDock = QtGui.QDockWidget(str(name), self)
    viewportDock.setObjectName(name)
    viewportDock.setWidget(intermediateOwnerWidget)
    #viewportDock.setFeatures(self.dockFeatures)
    viewportDock.setAttribute(QtCore.Qt.WA_DeleteOnClose)
    self.addDockWidget(QtCore.Qt.TopDockWidgetArea, viewportDock)
    
  def onAddViewport(self):
    self.__addViewport(False)

  def onAddOrthoViewport(self):
    self.__addViewport(True)

  def onSetSamples(self):
    for i in range(0, 4):
      if(self.sampleActions[i] is self.sender()):
        self.samples = math.pow(2, i)
        self.__updateSampleChecks()

        # We need to recreate all viewports widgets with the new sampling setting
        oldViewports = list(self.viewports)
        for j in range(0, len(oldViewports)):
          viewport = oldViewports[j]
          viewportIndex = viewport.getViewportIndex()
          newViewport, _ = self.__createViewport(viewportIndex, viewport.isOrthographic(), viewport)
          if(viewportIndex == 0): self.viewport = newViewport

  def onRefreshAllViewports(self):
    self.shMainGLScene.prepareSceneForRender()
    for viewport in self.viewports: viewport.redraw()

  def onRefreshViewport(self, refreshAll):
    if refreshAll == True : self.onRefreshAllViewports()
    else:
      viewport = self.sender()
      viewport.redraw()

  def onViewportDestroying(self):
    viewport = self.sender()
    if viewport in self.viewports: self.viewports.remove(viewport)

  def onTogglePlayback(self):  
    if self.timeLineDock.isVisible() == False : self.timeLineDock.show()
    self.timeLine.play()
   
  def onFrameChanged(self, frame):
    super(SceneHubWindow, self).onFrameChanged(frame)
    self.shMainGLScene.setFrame(frame)
    self.onRefreshAllViewports()

  def onPlaybackChanged(self, isPlaying):
    self.shGLRenderer.setPlayback(isPlaying)
    # Refresh viewports: because there might be a "one frame behind" if were are drawing while computing
    self.onRefreshAllViewports()


  def onImportAsset(self): 
    self.timeLine.pause();
    #SHEditorWidget.ImportAsset(slef.shMainGLScene);

  def onExportToAlembic(self): 
    self.timeLine.pause();
    #FabricUI::SceneHub::SHEditorWidget::ExportToAlembic(m_shMainGLScene);

