import optparse, os, sys, math, copy
from FabricEngine import Core, FabricUI
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine.FabricUI import *
  
class SHGLViewportWidget(Viewports.RTRGLViewportWidget):
  def __init__(self, client, shGLRenderer, shGLScene, viewportIndex, context, parent, sharedWidget, setting):
    
    # Need to hold the context
    self.qglContext = context

    super(SHGLViewportWidget, self).__init__(
      client, 
      shGLRenderer,
      shGLScene,
      viewportIndex,
      self.qglContext,
      parent,
      sharedWidget,
      setting)
