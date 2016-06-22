import math
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine import Core
from FabricEngine.FabricUI import *
from FabricEngine.SceneHub.SHViewport import SHViewport


class SHViewportDock(QtGui.QDockWidget):

    """SHViewportDock

    SHViewportsManager specialized QtGui.QDockWidget.
    When the dock is closed, an event is emitted so the viewport the dock owns
    can be properly deleted.
    
    Arguments:
        viewportIndex (int): The internal viewport index (used in KL).
        parent (QMainWindow): A reference to the QDock parent.
    """

    deleteViewport = QtCore.Signal(int)

    def __init__(self, viewportIndex, parent):
        self.viewportIndex = viewportIndex
        name = str("Viewport " + str(viewportIndex))
        super(SHViewportDock, self).__init__(name, parent)
        self.setObjectName(name)

    def closeEvent(self, event):
        self.deleteViewport.emit(self.viewportIndex)


class SHViewportsManager():

    """SHViewportsManager

    SHViewportsManager defines all the viewports functionalities needed by SceneHubWindows.
    It manages the viewports :
        - Add Perspective/Orthigraphic 
        - Delete
    and the SceneHub::SHGLRenderer.

 
    Arguments:
        mainwindow (FabricEngine.Core.Client): A reference to the FabricCore.Client.
        shStates (SceneHub.SHStates): A reference to the SHStates.
        shRenderer (SceneHub.SHGLRenderer): A reference to the SHGLRenderer.
        samples (int): Initiale number of anti-aliasing samples, [1, 2, 4, 8]
    """

    def __init__(self, mainwindow, shStates, shRenderer, samples):
        self.samples = samples
        self.shStates = shStates
        self.shWindow = mainwindow
        self.viewports = []
        self.sampleActions = []
        self.nextViewportIndex = 1
        self.shGLRenderer = SceneHub.SHGLRenderer(self.shWindow.client, shRenderer)

        self.shStates.sceneChanged.connect(self.onRefreshAllViewports)
        self.shStates.selectionChanged.connect(self.onRefreshAllViewports)

        # Manips can need to be redrawn even if the scene didn't change
        self.shGLRenderer.manipsAcceptedEvent.connect( self.onRefreshAllViewports )

    def initMenu(self, menuBar):
        """ Initializes the application menu reltive to the viewports.
        """
        menus = menuBar.findChildren(QtGui.QMenu)
        for menu in menus:
            if menu.title() == "&Window":
                viewportMenu = menu.addMenu("Add Viewport")
                viewportAction = viewportMenu.addAction("Add Perspective Viewport")
                viewportAction.triggered.connect(self.__onAddViewport)
                viewportAction = viewportMenu.addAction("Add Orthographic Viewport")
                viewportAction.triggered.connect(self.__onAddOrthoViewport)

                multisampleMenu = menu.addMenu("Multisampling")
                for i in range(0, 4):
                    self.sampleActions.append(multisampleMenu.addAction(str(int(math.pow(2, i)))))
                    self.sampleActions[i].setCheckable(True)
                    self.sampleActions[i].triggered.connect(self.__onSetSamples)
                self.__updateSampleChecks()
    
    def update(self):
        """ Force the SHGLRenderer to update.
        """

        self.shGLRenderer.update()

    def getSHRenderer(self):
        return self.shGLRenderer

    def createViewport(self, index, ortho, replace, sharedWidget):
        """ Creates a viewport when adding or replacing one.
        The method is also in charge of creating the 


        Arguments:
            index (int): The viewport index.
            ortho (bool): If true, create an orthographic viewport
            replace (bool): If true, replace the current viewport
            sharedWidget (SceneHub.SHViewport): A reference to an existing viewport : share context, can be None.

        """

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

        # Manips can need to be redrawn even if the scene didn't change
        newViewport.redrawOnAlwaysRefresh.connect( self.onRefreshAllViewports, QtCore.Qt.QueuedConnection )

        return newViewport, intermediateOwnerWidget;

    def __onDeleteViewport(self, viewportIndex):
        """ Deletes the viewport viewportIndex.

        Arguments:
            viewportIndex (int): Viewport's getViewportIndex()
        """
        for i in range(len(self.viewports)):
          if self.viewports[i].getViewportIndex() == viewportIndex:
            self.viewports[i].detachFromRTRViewport()
            del self.viewports[i]
            break

    def __addViewport(self, orthographic):
        """ Adds a new viewport.

        Arguments:
            orthographic (bool): If true, create an orthographic viewport
        """
 
        viewportIndex = self.nextViewportIndex
        self.nextViewportIndex = self.nextViewportIndex + 1
        _, intermediateOwnerWidget = self.createViewport(
          viewportIndex, 
          orthographic, 
          False, 
          self.shWindow.viewport)
     
        viewportDock = SHViewportDock(viewportIndex, self.shWindow)
        viewportDock.setWidget(intermediateOwnerWidget)
        viewportDock.setFloating( True )
        viewportDock.deleteViewport.connect(self.__onDeleteViewport)
        self.shWindow.addDockWidget(QtCore.Qt.TopDockWidgetArea, viewportDock)

    def __updateSampleChecks(self):
        """ Updates the anti-aliasing number of samples.
        """
        self.sampleActions[0].setChecked(False)
        self.sampleActions[1].setChecked(False)
        self.sampleActions[2].setChecked(False)
        self.sampleActions[3].setChecked(False)
        if( self.samples == 1 ):   self.sampleActions[0].setChecked(True)
        elif( self.samples == 2 ): self.sampleActions[1].setChecked(True)
        elif( self.samples == 4 ): self.sampleActions[2].setChecked(True)
        elif( self.samples == 8 ): self.sampleActions[3].setChecked(True)

    def onPlaybackChanged(self, isPlaying):
        """ Toggles when playback changed.

        Arguments:
            isPlaying (bool): Playback active or not 
        """

        self.shGLRenderer.setPlayback(isPlaying)
        # Refresh viewports: because there might be a "one frame behind" 
        # if were are drawing while computing
        self.onRefreshAllViewports()
      
    def onRefreshAllViewports(self):
        """ Refreshs all the viewports.
        """

        self.shWindow.shTreesManager.getScene().prepareSceneForRender()
        for viewport in self.viewports: viewport.update()

    def viewportUpdateRequested(self):
        for viewport in self.viewports:
            if viewport.updateRequested:
                return True
        return False

    def onRefreshViewport(self, refreshAll):
        """ Refreshs either the current viewport or all the viewports.

        Arguments:
            refreshAll (bool): If true, refresh all the viewports
        """

        if refreshAll == True: 
            self.onRefreshAllViewports()
        else:
            viewport = self.sender()
            viewport.update()

    def onAlwaysRefresh(self):
        """ Enables alwaysRefresh all viewports.
        """
        for viewport in self.viewports:
            viewport.toggleAlwaysRefresh()

    def __onAddViewport(self):
        """ Adds a viewport with perspective camera.
        """

        self.__addViewport(False)

    def __onAddOrthoViewport(self):
        """ Adds a viewport with orthographic camera.
        """

        self.__addViewport(True)

    def __onSetSamples(self):
        """ Updates the number of anti-aliasing samples.

        """

        for i in range(0, 4):
            if self.sampleActions[i].isChecked() and self.samples != i:
                self.samples = i
                self.__updateSampleChecks()

                # We need to recreate all viewports widgets with the new sampling setting
                oldViewports = list(self.viewports)
                for j in range(0, len(oldViewports)):
                    viewport = oldViewports[j]
                    index = viewport.getViewportIndex()
                    orthographic = viewport.isOrthographic();
              
                    newViewport, _ = self.createViewport(index, orthographic, True, viewport)
                    if(index == 0): 
                        self.shWindow.viewport = newViewport

                break
