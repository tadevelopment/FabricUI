import math
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine import Core
from FabricEngine.FabricUI import *
from SHViewport import SHViewport

class SHViewportsManager():

  def __init__(self, mainwindow):
    self.samples = 1
    self.shWindow = mainwindow
    self.viewports = []
    self.sampleActions = []
    self.nextViewportIndex = 1
    self.shGLRenderer = SceneHub.SHGLRenderer(self.shWindow.client)

  def initMenu(self, menuBar):
    menus = menuBar.findChildren(QtGui.QMenu)
    for menu in menus:
      if menu.title() == "&Window":
        viewportMenu = menu.addMenu("Add Viewport")
        viewportAction = viewportMenu.addAction("Add Perspective Viewport")
        viewportAction.triggered.connect(self._onAddViewport)
        viewportAction = viewportMenu.addAction("Add Orthographic Viewport")
        viewportAction.triggered.connect(self._onAddOrthoViewport)

        multisampleMenu = menu.addMenu("Multisampling")
        for i in range(0, 4):
          self.sampleActions.append(multisampleMenu.addAction(str(int(math.pow(2, i)))))
          self.sampleActions[i].setCheckable(True)
          self.sampleActions[i].triggered.connect(self._onSetSamples)
        self._updateSampleChecks()
    
  def update(self):
    self.shGLRenderer.update()

  def createViewport(self, index, ortho, replace, sharedWidget):
    # If the multisampling has changed, we need to replace the RTRGLViewportWidget with a new one, since there
    # is a 1-to-1 correspondance between the QGlWidget, its context, and context's sampling options.
    # To do it, we add an intermediate widget, otherwise the layout collapses when one widget with another one.
    qglContext = None
    intermediateLayout = None
    intermediateOwnerWidget = None
    
    if not replace:
      # initializing
      intermediateOwnerWidget = QtGui.QWidget(self.shWindow)
      intermediateLayout = QtGui.QStackedLayout()
      intermediateOwnerWidget.setLayout(intermediateLayout)
       
      format = QtOpenGL.QGLFormat()
      format.setSamples(self.samples)
      format.setSampleBuffers(self.samples > 1)
      qglContext = Viewports.RTRGLContext(format)

    else:
      intermediateOwnerWidget = sharedWidget.parent()
      intermediateLayout = intermediateOwnerWidget.layout()
      # We will recreate the RTR viewport associated with the index
      sharedWidget.detachFromRTRViewport()
      # create a context from the previous one, but change samples
      format = sharedWidget.context().format()
      format.setSamples(self.samples)
      format.setSampleBuffers(self.samples > 1)
      qglContext = Viewports.RTRGLContext(format)

    scene = self.shWindow.shTreesManager.getScene()
    newViewport = SHViewport(
      self.shGLRenderer,
      scene,
      index, 
      ortho,
      qglContext, 
      self.shWindow, 
      sharedWidget)

    intermediateLayout.addWidget(newViewport)

    if replace:
      index = self.viewports.index(sharedWidget)
      intermediateLayout.removeWidget(sharedWidget)
      self.viewports.remove(sharedWidget)
      sharedWidget.deleteLater()
    self.viewports.append(newViewport)

    newViewport.viewportDestroying.connect(self._onViewportDestroying)
    newViewport.sceneChanged.connect(self.onRefreshAllViewports)
    newViewport.manipsAcceptedEvent.connect(self.onRefreshAllViewports)

    self.shWindow.shTreesManager.sceneHierarchyChanged.connect(self.onRefreshAllViewports)
    self.shWindow.shTreesManager.sceneUpdated.connect(newViewport.onSceneUpdated)
    newViewport.sceneChanged.connect(self.shWindow.shTreesManager.onSceneHierarchyChanged)
    newViewport.manipsAcceptedEvent.connect(self.shWindow.shTreesManager.onUpdateFrom3DSelection)
    
    self.shWindow.qUndoStack.indexChanged.connect(self.onRefreshAllViewports)
    newViewport.addCommands.connect(self.shWindow.shCmdHandler.onAddCommands)
    
    #if self.shWindow.valueEditor is not None:
    #  newViewport.sceneChanged.connect(self.shWindow.valueEditor.onSceneChanged);

    return newViewport, intermediateOwnerWidget;

  def _addViewport(self, orthographic):
    index = self.nextViewportIndex
    self.nextViewportIndex = self.nextViewportIndex + 1
    _, intermediateOwnerWidget = self.createViewport(index, orthographic, False, self.shWindow.viewport)
    
    name = str("Viewport " + str(index))
    viewportDock = QtGui.QDockWidget(name, self.shWindow)
    viewportDock.setObjectName(name)
    viewportDock.setWidget(intermediateOwnerWidget)
    #viewportDock.setFeatures(self.dockFeatures)
    viewportDock.setAttribute(QtCore.Qt.WA_DeleteOnClose)
    self.shWindow.addDockWidget(QtCore.Qt.TopDockWidgetArea, viewportDock)
    
  def _updateSampleChecks(self):
    self.sampleActions[0].setChecked(False)
    self.sampleActions[1].setChecked(False)
    self.sampleActions[2].setChecked(False)
    self.sampleActions[3].setChecked(False)
    if( self.samples == 1 ):   self.sampleActions[0].setChecked(True)
    elif( self.samples == 2 ): self.sampleActions[1].setChecked(True)
    elif( self.samples == 4 ): self.sampleActions[2].setChecked(True)
    elif( self.samples == 8 ): self.sampleActions[3].setChecked(True)

  def _onPlaybackChanged(self, isPlaying):
    self.shGLRenderer.setPlayback(isPlaying)
    # Refresh viewports: because there might be a "one frame behind" if were are drawing while computing
    self.onRefreshAllViewports()

  def _onViewportDestroying(self):
    pass
    #del self.viewports[index]
    #viewport = self.sender()
    #if i, viewport in self.viewports: 
    #  self.viewports.remove(viewport)
  
  def onRefreshAllViewports(self):
    self.shWindow.shTreesManager.getScene().prepareSceneForRender()
    for viewport in self.viewports: viewport.update()

  def onRefreshViewport(self, refreshAll):
    if refreshAll == True: 
      self.onRefreshAllViewports()
    else:
      viewport = self.sender()
      viewport.update()
 
  def _onAddViewport(self):
    self._addViewport(False)

  def _onAddOrthoViewport(self):
    self._addViewport(True)

  def _onSetSamples(self):
    for i in range(0, 4):
      if self.sampleActions[i].isChecked() and self.samples != int(math.pow(2, i)):
 
        self.samples = int(math.pow(2, i))
        self._updateSampleChecks()

        # We need to recreate all viewports widgets with the new sampling setting
        oldViewports = list(self.viewports)
        for j in range(0, len(oldViewports)):
          viewport = oldViewports[j]
          index = viewport.getViewportIndex()
          orthographic = viewport.isOrthographic();
          
          newViewport, _ = self.createViewport(index, orthographic, True, viewport)
          if(index == 0): self.shWindow.viewport = newViewport

  def onSceneUpdated(self, scene):
    for i in range(0, len(self.viewports)):
      self.viewports[i].onSceneUpdated(scene)
