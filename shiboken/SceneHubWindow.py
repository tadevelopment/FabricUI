import os, sys, math
from FabricEngine import Core, FabricUI
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine.FabricUI import *
from CanvasWindow import CanvasWindow
from SHTreeViewWidget import SHTreeViewWidget
from SHViewportWidgetsManager import SHViewportWidgetsManager
from SHAssetsMenu import SHAssetsMenu
from SHLightsMenu import SHLightsMenu

class SceneHubWindow(CanvasWindow):

  def __init__(self, settings, unguarded, noopt, klFile):
    self.noopt = noopt
    self.klFile = klFile
    self.viewport = None
    self.isCanvas = False
    super(SceneHubWindow, self).__init__(settings, unguarded)

  def _initKL(self, unguarded):
    super(SceneHubWindow, self)._initKL(unguarded)
    self.client.loadExtension('SceneHub')
    # Create the renderer
    self.viewportsManager = SHViewportWidgetsManager(self)

  def _initTreeView(self):
    super(SceneHubWindow, self)._initTreeView()
    self.shTreeViewWidget = SHTreeViewWidget(self, self.klFile)
    # Update the renderer in case it has been overriden by the main scene.
    self.viewportsManager.update()
    self.shTreeViewWidget.sceneUpdated.connect(self.onSceneUpdated)

  def _initGL(self):
    self.viewport, intermediateOwnerWidget = self.viewportsManager.createViewport(0, False, False, None)
    self.setCentralWidget(intermediateOwnerWidget)
    self.viewport.makeCurrent()  

  def _initDocksAndMenus(self):
    super(SceneHubWindow, self)._initDocksAndMenus()
    shTreeDock = QtGui.QDockWidget("Sh Tree-View", self)
    shTreeDock.setObjectName("SH Tree-View")
    shTreeDock.setFeatures(self.dockFeatures)
    shTreeDock.setWidget(self.shTreeViewWidget)
    self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, shTreeDock, QtCore.Qt.Vertical)

    menuBar = self.menuBar()
    menus = menuBar.findChildren(QtGui.QMenu)
    for menu in menus:      
      if menu.title() == "&Window":
        toggleAction = shTreeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_8)
        menu.addAction(toggleAction)
        menu.addSeparator();
    
    self.viewportsManager.initMenu(menuBar)

    self.assetMenu = SHAssetsMenu(self.shTreeViewWidget.getScene())
    self.lightsMenu = SHLightsMenu(self.shTreeViewWidget.getScene())
    menus = menuBar.findChildren(QtGui.QMenu)    
    for menu in menus:      
      if menu.title() == "&File":
        actions = menu.findChildren(QtGui.QAction)
        for action in actions:
          if action.text() == "Quit":
            menu.insertMenu(action, self.assetMenu)
            menu.insertMenu(action, self.lightsMenu)

  def _contentChanged(self) :
    self.valueEditor.onOutputsChanged()
    self.viewportsManager.onRefreshAllViewports()

  def onTogglePlayback(self):  
    if self.timeLineDock.isVisible() == False : self.timeLineDock.show()
    self.timeLine.play()
   
  def onFrameChanged(self, frame):
    super(SceneHubWindow, self).onFrameChanged(frame)
    self.shTreeViewWidget.getScene().setFrame(frame)
    self.viewportsManager.onRefreshAllViewports()

  def onSceneUpdated(self, scene):
    self.viewportsManager.onSceneUpdated(scene)
    self.assetMenu.onSceneUpdated(scene)
    self.lightsMenu.onSceneUpdated(scene)