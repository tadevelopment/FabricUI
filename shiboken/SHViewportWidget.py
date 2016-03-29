import math
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine import Core
from FabricEngine.FabricUI import *  
from SHContextualMenu import SHContextualMenu

class SHViewportWidget(Viewports.ViewportWidget):
  sceneChanged = QtCore.Signal()
  viewportDestroying = QtCore.Signal()
  synchronizeCommands = QtCore.Signal()
  manipsAcceptedEvent = QtCore.Signal(bool)

  def __init__(self, renderer, scene, index, orthographic, context, parent, sharedWidget):
    # Need to hold the context
    self.orthographic = orthographic
    self.shGLScene = scene
    self.qglContext = context
    self.viewportIndex = index
    self.viewport = None
    self.orthographic = False
    self.alwaysRefresh = False
    self.shGLRenderer = renderer

    client = renderer.getClient()
    super(SHViewportWidget, self).__init__(client, QtGui.QColor(), self.qglContext, parent, sharedWidget, parent.settings)

    self.samples = self.qglContext.format().samples()
    # Force to track mouse movment when not clicking
    self.setMouseTracking(True)
    self.setAcceptDrops(True)
    self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
    self.viewport = self.shGLRenderer.getOrAddViewport(self.viewportIndex)

  def detachFromRTRViewport(self):
    if(self.viewportIndex > -1): self.shGLRenderer.removeViewport(self.viewportIndex)
    self.viewportIndex = -1
  
  def closeEvent(self): 
    detachFromRTRViewport()
    self.viewportDestroying.emit(self.viewportIndex)
   
  def setOrthographic(self, orthographic): 
    self.orthographic = orthographic
    self.shGLRenderer.setOrthographicViewport(self.viewportIndex, self.orthographic)
  
  def paintGL(self):
    self.computeFPS()
    self.shGLRenderer.render(self.viewportIndex, self.width, self.height, self.samples)
    if(self.alwaysRefresh): self.update()
  
  def resizeGL(self, width, height): 
    self.width = width
    self.height = height
  
  def toggleAlwaysRefresh(self):
    if self.alwaysRefresh:self.alwaysRefresh = False
    else: self.alwaysRefresh = True

  def isOrthographic(self):
    return self.orthographic

  def getViewportIndex(self):
    return self.viewportIndex
 
  def alwaysRefreshes(self):
    return self.alwaysRefresh

  def getCamera(self): 
    return self.shGLRenderer.getCamera(self.viewportIndex)

  def enterEvent(self, event): 
    self.grabKeyboard()

  def leaveEvent(self, event): 
    self.releaseKeyboard()

  def keyPressEvent(self, event):
    self.__onEvent(event)

  def keyReleaseEvent(self, event):
    self.__onEvent(event)

  def wheelEvent(self, event):
    self.__onEvent(event)

  def mouseMoveEvent(self, event):
    self.__onEvent(event)

  def mouseReleaseEvent(self, event):
    self.__onEvent(event)
 
  def mousePressEvent(self, event):
    if self.shGLScene.hasSG():
      if not self.__onEvent(event) and event.button() == QtCore.Qt.RightButton:
        menu = SHContextualMenu(self.shGLScene, "")
        menu.exec_(self.mapToGlobal(event.pos()))
        self.sceneChanged.emit();
    
  def __onEvent(self, event):
    redrawAllViewports = False
    if self.shGLRenderer.onEvent(self.viewportIndex, event, redrawAllViewports, False):
      self.manipsAcceptedEvent.emit(redrawAllViewports)
      return True
    return False
  
  def dragEnterEvent(self, event):
    if event.mimeData().hasUrls() and (event.possibleActions() & QtCore.Qt.CopyAction):
      event.setDropAction(QtCore.Qt.CopyAction)
      event.accept()

  def dragMoveEvent(self, event): 
    if(event.mimeData().hasUrls() and (event.possibleActions() & QtCore.Qt.CopyAction)):
      # Convert to a mouseMove event
      mouseEvent = QtGui.QMouseEvent(QtGui.QEvent.MouseMove, event.pos(), QtCore.Qt.LeftButton, QtCore.Qt.LeftButton, QtCore.Qt.NoModifier)
      redrawAllViewports = False
      if self.shGLRenderer.onEvent(self.viewportIndex, mouseEvent, redrawAllViewports, True):
        self.manipsAcceptedEvent.emit(redrawAllViewports)

  def dropEvent(self, event):
    if(self.shGLScene)
      const QMimeData *myData = qobject_cast<const QMimeData*>(event.mimeData());
      if(!myData) return;
      if(!event.mimeData().hasUrls()) return;

      bool forceExpand = event.keyboardModifiers() & QtCore.Qt.ControlModifier;
      QStringList pathList;
      foreach(QUrl url, event.mimeData().urls())
        pathList.append(url.toLocalFile());
          
      if(pathList.size() == 0) return;
       
      float pos3D[3];
      self.shGLRenderer.get3DScenePosFrom2DScreenPos( self.viewportIndex, event.pos(), pos3D );
      SHEditorWidget::AddExternalFileList(self.shGLScene, pathList, pos3D, forceExpand);
      event.acceptProposedAction();
      self.sceneChanged.emit();

  def onSceneUpdated(self, scene):
    self.shGLScene = scene
