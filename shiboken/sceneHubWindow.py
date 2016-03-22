import optparse, os, sys
from FabricEngine import Core, FabricUI
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine.FabricUI import *
from canvasWindow import CanvasWindow
 
class SceneHubWindow(CanvasWindow):

    def __init__(self, settings, unguarded, qglContext):
        super(SceneHubWindow, self).__init__(settings, unguarded)

        self.qglContext = qglContext

    def initKL(self, unguarded):
        super(SceneHubWindow, self).initKL(unguarded)

        self.client.loadExtension('Manipulation')
        self.client.loadExtension('SceneHub')
        # Create the renderer, only one.
        self.shGLRenderer = SceneHub.SHGLRenderer(self.client)
        # Construct the "main scene"
        sceneName = 'SceneHub'
        #if(klFile.length() > 0) sceneName = loadScene(self.client, klFile)
        self.shMainGLScene = SceneHub.SHGLScene(self.client, sceneName)
        # Update the renderer in case it has been overriden by the main scene.
        self.shGLRenderer.update()

    def initGL(self):
        self.samples = self.qglContext.format().samples()
        self.viewport, intermediateOwnerWidget = createViewport( 0, false, 0, qglContext, intermediateOwnerWidget )
        setCentralWidget(intermediateOwnerWidget)
        self.viewport.makeCurrent()

    def initTreeView(self):
        super(SceneHubWindow, self).initTreeView()

        self.shTreeViewWidget = SceneHub.SHTreeViewWidget(
            self.shMainGLScene,
            self.dfgWidget.getUIController())

    def initDocksAndMenus(self):
        super(SceneHubWindow, self).initDocksAndMenus()

        shTreeDock = QtGui.QDockWidget("Sh Tree-View", self)
        shTreeDock.setObjectName("SH Tree-View")
        shTreeDock.setWidget(self.shTreeViewWidget)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, shTreeDock, QtCore.Qt.Vertical)

        menus = self.menuBar().findChildren(QtGui.QMenu)
        for menu in menus:
            if menu.title() == "&Window":
                print menu.title()
                toggleAction = shTreeDock.toggleViewAction()
                toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_8)
                menu.addAction(toggleAction)
        
    def createViewport(
        self, 
        viewportIndex, 
        orthographic, 
        widgetToReplaceWithNewSampling,
        qglContext):

        # If the multisampling has changed, we need to replace the RTRGLViewportWidget with a new one, since there
        # is a 1-to-1 correspondance between the QGlWidget, its context, and context's sampling options.
        # To do this, we add an intermediate widget, otherwise the layout collapses when one widget with another one.
        intermediateLayout = None
        intermediateOwnerWidget = None
        if( widgetToReplaceWithNewSampling is None ) :
            # initializing
            intermediateOwnerWidget = QtGuiQWidget(self)
            intermediateLayout = QtGui.QStackedLayout()
            intermediateOwnerWidget.setLayout(intermediateLayout)

            if( qglContext is None ) :
                qglContext = RTRGLContext( self.viewport.context().format() )

        else :
            intermediateOwnerWidget = widgetToReplaceWithNewSampling.parentWidget()
            intermediateLayout = intermediateOwnerWidget.layout()

            # We will recreate the RTR viewport associated with the index
            widgetToReplaceWithNewSampling.detachFromRTRViewport()

            # create a context from the previous one,
            # but change samples
            format = widgetToReplaceWithNewSampling.context().format()
            format.setSamples( self.samples )
            format.setSampleBuffers( self.samples > 1 )
            qglContext = Viewports.RTRGLContext( format )

        if widgetToReplaceWithNewSampling is not None:
            widgetToReplaceWithNewSampling = widgetToReplaceWithNewSampling
        else: 
            widgetToReplaceWithNewSampling = self.viewport

        newViewport = Viewports.RTRGLViewportWidget(
            self.client,
            self.shGLRenderer,
            self.shTreeViewWidget.getScene(),
            viewportIndex, 
            qglContext, 
            self, 
            widgetToReplaceWithNewSampling, 
            self.settings)
     
        if( orthographic is not None ) :
            newViewport.setOrthographic(true)

        intermediateLayout.addWidget(newViewport)
        if( widgetToReplaceWithNewSampling is not None ) :
            self.viewports.erase( self.viewports.find( widgetToReplaceWithNewSampling ) )
            intermediateLayout.removeWidget(widgetToReplaceWithNewSampling)
            widgetToReplaceWithNewSampling.deleteLater()
      
        self.viewports.insert( newViewport )

        self.shTreeViewWidget.sceneHierarchyChanged.connect(self.refreshViewports())
        self.shTreeViewWidget.sceneUpdated.connect(self.onSceneUpdated());
        newViewport.sceneChanged().connect(self.shTreeViewWidget, SLOT( onSceneHierarchyChanged() ) )
        newViewport.viewportDestroying().connect(self.viewportDestroying() )
        newViewport.sceneChanged().connect(self.refreshViewports() )
        newViewport.manipsAcceptedEvent().connect(self.shTreeViewWidget.updateFrom3DSelection() )
        newViewport.manipsAcceptedEvent().connect(this.refreshViewports() )

        #if(self.shCmdViewWidget)
        #  newViewport.synchronizeCommands.connect( self.shCmdViewWidget.synchronize() )

        return newViewport, intermediateOwnerWidget;
     

'''
 


void SceneHubWindow::updateSampleChecks() {
  m_sampleAction[0].setChecked(false);
  m_sampleAction[1].setChecked(false);
  m_sampleAction[2].setChecked(false);
  m_sampleAction[3].setChecked(false);
  if( self.samples == 1 )
    m_sampleAction[0].setChecked(true);
  else if( self.samples == 2 )
    m_sampleAction[1].setChecked(true);
  else if( self.samples == 4 )
    m_sampleAction[2].setChecked(true);
  else if( self.samples == 8 )
    m_sampleAction[3].setChecked(true);
}

void SceneHubWindow::setSamples(int samples) {
  if( samples != self.samples ) 
  {
    self.samples = samples;
    updateSampleChecks();

    // We need to recreate all viewports widgets with the new sampling setting
    ViewportSet oldViewports = self.viewports;

    for( ViewportSet::iterator it = oldViewports.begin(); it != oldViewports.end(); ++it ) 
    {
      int viewportIndex = (*it).getViewportIndex();
      QWidget *intermediateOwnerWidget;
      FabricUI::Viewports::RTRGLViewportWidget* newViewport = createViewport( viewportIndex, (*it).isOrthographic(), (*it), 0, intermediateOwnerWidget );
      if( viewportIndex == 0 )
        self.viewport = newViewport;
    }
  }
}

void SceneHubWindow::addViewport(bool orthographic) {
  int index = m_nextViewportIndex++;
  QWidget *intermediateOwnerWidget;
  createViewport( index, orthographic, 0, 0, intermediateOwnerWidget );

  QString name = "Viewport " + QString::number(index);
  QDockWidget *viewportDock = new QDockWidget( name, this);
  viewportDock.setObjectName( name );
  viewportDock.setWidget( intermediateOwnerWidget );
  viewportDock.setAttribute(Qt::WA_DeleteOnClose);
  viewportDock.setFloating( true );
  addDockWidget( Qt::TopDockWidgetArea, viewportDock);
}

void SceneHubWindow::refreshViewports() {
  m_shMainGLScene.prepareSceneForRender();
  for(ViewportSet::iterator it = self.viewports.begin(); it != self.viewports.end(); ++it)
    (*it).redraw();
}

void SceneHubWindow::refreshViewports(bool refreshAll) {
  if( refreshAll ) refreshViewports();
  else {
    FabricUI::Viewports::ViewportWidget *viewport = (FabricUI::Viewports::ViewportWidget *)sender();
    viewport.redraw();
  }
}

void SceneHubWindow::viewportDestroying() {
  ViewportSet::iterator iter = self.viewports.find( (FabricUI::Viewports::RTRGLViewportWidget*)sender() );
  if( iter != self.viewports.end() )
    self.viewports.erase( iter );
}

void SceneHubWindow::togglePlayback() {
  if(!m_timeLineDock.isVisible()) m_timeLineDock.show();
  m_timeLine.play();
}

void SceneHubWindow::onFrameChanged(int frame) {
  CanvasWindow::onFrameChanged(frame);
  m_shMainGLScene.setFrame(frame);
}

void SceneHubWindow::onPlaybackChanged(bool isPlaying) {
  self.shGLRenderer.setPlayback(isPlaying);
  // Refresh viewports: because there might be a "one frame behind" if were are drawing while computing
  refreshViewports();
}
'''
