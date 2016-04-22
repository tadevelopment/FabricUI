from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI, Util, CAPI
from FabricEngine.FabricUI import *
from FabricEngine.Util import *
from FabricEngine.CAPI import *
from FabricEngine.SceneHub.SHTreeView import SHTreeView

class SHTreeComboBox(QtGui.QComboBox):
  constructSceneList = QtCore.Signal()

  def __init__(self): 
    super(SHTreeComboBox, self).__init__()

  def showPopup(self):
    self.constructSceneList.emit()
    super(SHTreeComboBox, self).showPopup()
 

class SHTreeViewsManager(QtGui.QWidget):
  sceneChanged = QtCore.Signal()
  sceneHierarchyChanged = QtCore.Signal()
  itemDoubleClicked = QtCore.Signal(SceneHub.SHTreeItem)
  activeSceneChanged = QtCore.Signal(SceneHub.SHGLScene)
  
  def __init__(self, client, dfgWidget, shStates, klFile):
    self.dfgWidget = dfgWidget
    self.shStates = shStates
    self.showProperties = True
    self.showOperators = True
    self.client = client
 
    super(SHTreeViewsManager, self).__init__()
    self.shMainGLScene = SceneHub.SHGLScene(client, klFile)
    self.shGLScene = SceneHub.SHGLScene(client)

    self.treeModel = None
    self.bUpdatingSelection = False

    self.comboBox = SHTreeComboBox()
    self.shTreeView = SHTreeView(self.client, self.shStates, self.shGLScene)
    
    layout = QtGui.QVBoxLayout()
    layout.addWidget(self.comboBox)
    layout.addWidget(self.shTreeView)
    self.setLayout(layout)
    self.comboBox.addItem("Main Scene")

    self.shTreeView.selectionCleared.connect(self.onSelectionCleared)
    self.shTreeView.itemSelected.connect(self.onTreeItemSelected)
    self.shTreeView.itemDeselected.connect(self.onTreeItemDeselected)
    self.shTreeView.itemDoubleClicked.connect(self.onTreeItemDoubleClicked)

    self.comboBox.constructSceneList.connect(self._onConstructSceneList)
    self.comboBox.currentIndexChanged.connect(self._onConstructScene)

    self.activeSceneChanged.connect( self.shStates.onActiveSceneChanged )

    self._onConstructScene()
 
  def getScene(self):
    return self.shGLScene

  def _resetTree(self):
    self.shTreeView.reset()
    if self.treeModel is not None: self.treeModel = None
 
  def _constructTree(self):
    self._resetTree()

    self.treeModel = SceneHub.SHTreeModel(
      self.shStates.getClient(), 
      self.shGLScene.getSG(), 
      self.shTreeView)

    self.setShowProperties(self.showProperties)
    self.setShowOperators(self.showOperators)

    self.sceneHierarchyChanged.connect(self.treeModel.onSceneHierarchyChanged)
    self.treeModel.sceneHierarchyChanged.connect(self.onSceneHierarchyChanged)
    self.treeModel.sceneChanged.connect(self.onSceneChanged)

    sceneRootIndex = self.treeModel.addRootItem(self.shGLScene.getSceneRoot())
    self.treeModel.addRootItem(self.shGLScene.getAssetLibraryRoot())
    self.treeModel.addRootItem(self.shGLScene.getMaterialLibraryRoot())
    self.treeModel.addRootItem(self.shGLScene.getImageLibraryRoot())

    self.shTreeView.setModel(self.treeModel)
    self.shTreeView.setExpanded(sceneRootIndex, True)

  def setShowProperties(self, show):
    self.showProperties = show
    if self.treeModel is not None:
      self.treeModel.setShowProperties(show)

  def setShowOperators(self, show):
    self.showOperators = show
    if self.treeModel is not None:
      self.treeModel.setShowOperators(show)

  def _onConstructScene(self):
    sceneName = self.comboBox.currentText()
    if str(sceneName) == "Main Scene":
      self.shGLScene.setSHGLScene(self.shMainGLScene)
      self._constructTree()
      self.activeSceneChanged.emit(self.shGLScene)
    
    elif self.dfgWidget.getDFGController().getBinding().getExec().hasVar(str(sceneName)):
      self.shGLScene.setSHGLScene(self.dfgWidget.getDFGController().getBinding(), sceneName)
      self._constructTree()
      self.activeSceneChanged.emit(self.shGLScene)
    
    else:
      self.comboBox.clear()
      self._resetTree()
    
  def _onConstructSceneList(self):
    self.comboBox.clear()
    binding = self.dfgWidget.getDFGController().getBinding()
    sceneNameList = self.shGLScene.getSceneNamesFromBinding(binding)
    if len(sceneNameList) == 0 and not self.shGLScene.hasSG(): 
      self._resetTree()
    if self.shMainGLScene.hasSG(): 
      self.comboBox.addItem("Main Scene")
    for sceneName in sceneNameList: 
      self.comboBox.addItem(sceneName)
    
  def expandTree(self, level):
    # Out of range unsigned int in KL.
    if level == 4294967295: 
      self.shTreeView.expandAll()
    else: 
      if level > 0: 
        self.shTreeView.expandToDepth(level-1)

  def onSceneHierarchyChanged(self):
    self.sceneHierarchyChanged.emit()

  def onSceneChanged(self):
    self.sceneChanged.emit()

  def onSelectionCleared(self):
    if not self.bUpdatingSelection:
      self.shStates.clearSelection()

  def onTreeItemSelected(self, item):
    if not self.bUpdatingSelection:
      self.bUpdatingSelection = True
      if item.isReference():
        val = item.getSGObject()
        if val is not None:
          self.shStates.addSGObjectToSelection(val)
      else:
        val = item.getSGObjectProperty()
        if val is not None:
          if item.isGenerator():
            self.shStates.addSGObjectPropertyGeneratorToSelection(val)
          else:
            self.shStates.addSGObjectPropertyToSelection(val)
      self.bUpdatingSelection = False
     
  def onTreeItemDeselected(self, item):
    if not self.bUpdatingSelection:
      self.bUpdatingSelection = True

      if item.isReference():
        val = item.getSGObject()
        if val is not None:
          self.shStates.removeSGObjectFromSelection(val)
      else:
        val = item.getSGObjectProperty()
        if val is not None:
          if item.isGenerator():
            self.shStates.removeSGObjectPropertyGeneratorFromSelection(val)
          else:
            self.shStates.removeSGObjectPropertyFromSelection(val)
      self.bUpdatingSelection = False

  def onTreeItemDoubleClicked(self, item):
    if not self.bUpdatingSelection:
      if item.isReference():
        val = item.getSGObject()
        if val is not None:
          self.shStates.onInspectedSGObject(val)
      else:
        val = item.getSGObjectProperty()
        if val is not None:
          if item.isGenerator():
            self.shStates.onInspectedSGObjectPropertyGenerator(val)
          else:
            self.shStates.onInspectedSGObjectProperty(val)

  def onSelectionChanged(self):
    if not self.bUpdatingSelection:
      # Ensure there's not a loopback when the selection is being updated
      self.bUpdatingSelection = True
      self.shTreeView.setSelectedObjects(self.shStates.getSelectedObjects())
      self.bUpdatingSelection = False
