
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from SHBaseMenu import SHBaseMenu

class SHInteractionMenu(SHBaseMenu):
  togglePlayback= QtCore.Signal()

  def __init__(self, renderer, title = None, parent = None):
    self.shGLRenderer = renderer
    super(SHInteractionMenu, self).__init__(None, "Tools", parent)
 
  def _constructMenu(self):
    tools = self.shGLRenderer.getRegisteredTools()
    for i in range(0, len(tools[0])):
      toolAction = self.addAction(tools[0][i] + "\t" + tools[1][i])
      toolAction.triggered.connect(self.activeTool)
    
  def activeTool(self):
    action = self.sender()
    actionName = str(action.text())

    pos = actionName.find("\t")     
    toolName = actionName[0:pos] 
    toolKey = actionName[pos:len(actionName)] 

    toolKey = toolKey.strip()
    seq = QtGui.QKeySequence(toolKey)
    qtKey = QtGui.QKeyEvent(QtCore.QEvent.KeyPress, seq[0], QtCore.Qt.NoModifier)
     
    redrawAllViewports = bool()
    #Viewport 0
    self.shGLRenderer.onEvent(0, qtKey, redrawAllViewports, False)
 