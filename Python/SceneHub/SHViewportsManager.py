import math
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine import Core
from FabricEngine.FabricUI import *
from FabricEngine.SceneHub.SHViewport import SHViewport


class SHViewportDock(QtGui.QDockWidget):
  deleteViewport = QtCore.Signal(int)

  def __init__(self, index, viewportIndex, parent):
    self.index = index
    name = str("Viewport " + str(viewportIndex))
    super(SHViewportDock, self).__init__(name, parent)
    self.setObjectName(name)

  def closeEvent(self, event):
    self.deleteViewport.emit(self.index)


class SHViewportsManager():

  def __init__(self, mainwindow, shStates, samples):
    self.samples = samples
    self.shStates = shStates
    self.shWindow = mainwindow
    self.viewports = []
    self.sampleActions = []
    self.nextViewportIndex = 1
    self.shGLRenderer = SceneHub.SHGLRenderer(self.shWindow.client)

    self.shStates.sceneChanged.connect(self.onRefreshAllViewports)
    self.shStates.selectionChanged.connect(self.onRefreshAllViewports)

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

    newViewport = SHViewport(
      self.shGLRenderer,
      self.shStates,
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

    newViewport.sceneChanged.connect( self.shStates.onStateChanged )
    newViewport.manipsAcceptedEvent.connect( self.shStates.onStateChanged )

    # Manips can need to be redrawn even if the scene didn't change
    newViewport.manipsAcceptedEvent.connect( self.onRefreshAllViewports )
    newViewport.synchronizeCommands.connect(self.shWindow.shCmdHandler.onSynchronizeCommands)

    return newViewport, intermediateOwnerWidget;

  def _onDeleteViewport(self, index):
    self.viewports[index].detachFromRTRViewport()
    del self.viewports[index]
    for i in range(0, len(self.viewports)):
      self.viewports[i].index = i

  def _addViewport(self, orthographic):
    viewportIndex = self.nextViewportIndex
    self.nextViewportIndex = self.nextViewportIndex + 1
    _, intermediateOwnerWidget = self.createViewport(
      viewportIndex, 
      orthographic, 
      False, 
      self.shWindow.viewport)
 
    viewportDock = SHViewportDock(len(self.viewports)-1, viewportIndex, self.shWindow)
    viewportDock.setWidget(intermediateOwnerWidget)
    viewportDock.setFloating( True )
    viewportDock.deleteViewport.connect(self._onDeleteViewport)
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

  def onPlaybackChanged(self, isPlaying):
    self.shGLRenderer.setPlayback(isPlaying)
    # Refresh viewports: because there might be a "one frame behind" if were are drawing while computing
    self.onRefreshAllViewports()
  
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
