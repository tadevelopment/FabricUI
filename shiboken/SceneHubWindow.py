import os, sys, math
from FabricEngine import Core, FabricUI
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine.FabricUI import *
from CanvasWindow import CanvasWindow
from SHTreeViewsManager import SHTreeViewsManager
from SHViewportsManager import SHViewportsManager
from SHAssetsMenu import SHAssetsMenu
from SHLightsMenu import SHLightsMenu
from SHTreeViewMenu import SHTreeViewMenu
 

class SceneHubWindow(CanvasWindow):

  def __init__(self, fabricDir, settings, unguarded, noopt, klFile):
    self.noopt = noopt
    self.klFile = klFile
    self.viewport = None
    self.shDFGCanvas = None
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
    self.shDFGCanvas = SceneHub.SHDFGCanvas(self.mainBinding, self.dfgWidget.getUIController(), self.client)
    self.shTreesManager.shTreeView.itemSelected.connect(self.shDFGCanvas.onTreeItemSelected)

  def _initTreeView(self):
    super(SceneHubWindow, self)._initTreeView()
    # Update the renderer in case it has been overriden by the main scene.
    self.viewportsManager.update()
    self.shTreesManager.sceneUpdated.connect(self.onSceneUpdated)
    self.shCmdHandler = SceneHub.SHCmdHandler(self.shTreesManager.getScene(), self.qUndoStack);
 
  def _initValueEditor(self):
    self.valueEditor = SceneHub.SHVEEditorOwner(self.dfgWidget, self.shTreesManager.shTreeView)
    if self.shTreesManager is not None:
      self.shTreesManager.sceneHierarchyChanged.connect(self.valueEditor.onSceneChanged)
      self.shTreesManager.sceneUpdated.connect(self.valueEditor.onSceneChanged)
    
    #for( ViewportSet::iterator it = m_viewports.begin(); it != m_viewports.end(); ++it )
    #  QObject::connect( *it, SIGNAL( sceneChanged() ), m_valueEditor, SLOT( onSceneChanged() ) );

    self.valueEditor.log.connect(self._onLog)
    self.valueEditor.modelItemValueChanged.connect(self._onModelValueChanged)
    self.valueEditor.canvasSidePanelInspectRequested.connect(self._onCanvasSidePanelInspectRequested)

  def _initGL(self):
    self.viewport, intermediateOwnerWidget = self.viewportsManager.createViewport(0, False, False, None)
    self.setCentralWidget(intermediateOwnerWidget)
    self.viewport.makeCurrent()  

  def _initDocksAndMenus(self):
    super(SceneHubWindow, self)._initDocksAndMenus()
    shTreeDock = QtGui.QDockWidget("Sh Tree-View", self)
    shTreeDock.setObjectName("SH Tree-View")
    shTreeDock.setFeatures(self.dockFeatures)
    shTreeDock.setWidget(self.shTreesManager)
    self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, shTreeDock, QtCore.Qt.Vertical)

    menus = self.menuBar().findChildren(QtGui.QMenu)
    for menu in menus:      
      if menu.title() == "&Window":
        toggleAction = shTreeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_8)
        menu.addAction(toggleAction)
        menu.addSeparator();
    
    self.viewportsManager.initMenu(self.menuBar())

    self.assetMenu = SHAssetsMenu(self.shTreesManager.getScene())
    self.lightsMenu = SHLightsMenu(self.shTreesManager.getScene())
    self.treeViewMenu = SHTreeViewMenu(self.shTreesManager)

    menus = self.menuBar().findChildren(QtGui.QMenu)    
    for menu in menus:      
      if menu.title() == "&File":
        actions = menu.findChildren(QtGui.QAction)
        for action in actions:
          if action.text() == "Quit":
            menu.insertMenu(action, self.assetMenu)
            menu.insertMenu(action, self.lightsMenu)

      if menu.title() == "&View":
        menu.addSeparator();
        menu.addMenu(self.treeViewMenu)

    interactionMenu = self.menuBar().addMenu("&Interaction")
    togglePlaybackAction = interactionMenu.addAction("Toggle Playback")
    togglePlaybackAction.triggered.connect(self._onTogglePlayback)
   
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
    if self.shDFGCanvas is not None and self.shDFGCanvas.isSgObjectValid(): 
      self.valueEditor.updateSGObject(self.shDFGCanvas)
  
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
    self.shCmdHandler.setScene(scene)

  def updateFPS(self):
    super(SceneHubWindow, self).updateFPS()
    '''
    FabricCore::RTVal scene = m_shMainGLScene->getSHGLScene();

    if ( !m_glWidget || !scene.isValid() ) return;
    
    QString caption;
    caption.setNum(m_glWidget->fps(), 'f', 2);
    caption += " FPS";
    
    uint32_t obj, point, line, triangle;
    # Viewport 0
    m_shGLRenderer->getDrawStats(0, obj, point, line, triangle);

    std::ostringstream ss;
    ss << " Drawn obj: " << obj;
    if( point ) ss << " pt: " << point;
    if( line ) ss << " li: " << line;
    if( triangle ) ss << " tri: " << triangle;
    caption += ss.str().c_str();
   
    self.fpsLabel.setText( caption );
    '''
  
  def _onShowUsage(self):
    txt = QtGui.QTextEdit()
    txt.setText("\
      SceneHub is a work-in-progress app framework for viewing and editing SceneGraph objects.\n\
      This is in an early 'alpha' state and can serve as an example or for testing.\n\
      \n\
      The following operations are supported:\n\
      \n\
      Tree view:\n\
      \n\
      - generated instances will be created as branches are expanded\n\
      - selection will be reflected in the 3D view, enabling the transform manipulator\n\
      - with an item selected, right-clicking will open a contextual menu to expand or load recursively\n\
      \n\
      3D view:\n\
      \n\
      - partially loaded assets are shown as bounding boxes (if at least one of the child isn't loaded)\n\
      \n\
      - hold 'Alt' key to navigate the camera:\n\
      \n\
        - click & drag: orbit\n\
        - middle-click & drag: pan\n\
        - wheel or right-click & drag: zoom\n\
        - arrow keys to orbit and zoom\n\
      \n\
      - click on a geometry to select it. This will activate the transform tool:\n\
      \n\
        - press S, R, T to change transform mode (might need to first click in the 3D view)\n\
        - click & drag the gizmos to change the transform\n\
      \n\
      - With a selected geometry, right-click to have a contextual menu for:\n\
      \n\
        - light: changing the color or intensity\n\
        - geometry: setting the color (shared), local color (instance specific) or texture\n\
        - parent: setting the color (propagated to children)\n\
      \n\
      - click in the background to un-select. Then, right-click to have a contextual menu for:\n\
      \n\
        - adding a geometry from a file (Fbx, Alembic)\n\
        - creating a light\n\
      \n\
      - drag & drop to the 3D view:\n\
      \n\
        - Fbx or Alembic file:\n\
          - adds the asset as a child of the 'root'\n\
          - bounding box'es bottom placed under mouse's cursor (raycast)\n\
          - assets dropped multiple times share their content in the SceneGraph (load once)\n\
          - if holding `Ctrl` key: will load recursively (expand)\n\
      \n\
        - Image file:\n\
          - set as a textured color if dropped on a geometry with UVs\n\
          - images dropped multiple times share their content in the SceneGraph (load once)\n\
          - if holding `Ctrl` key: will set as a local texture (instance specific)\n\
    ");

    txt.resize( 800, 500 );
    txt.setReadOnly( true );
    txt.show();
  
  def onDirty(self):
    if self.shDFGCanvas is not None and self.shDFGCanvas.dirtyAllOutputs():
      self.onRefreshAllViewports()

  def _onLog(self, message):
    if self.dfgWidget is not None:
      self.dfgWidget.getUIController().logError(message)
 
