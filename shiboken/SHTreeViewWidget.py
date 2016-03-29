from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from SHTreeView import SHTreeView
from SHAssetsMenu import SHAssetsMenu
from SHLightsMenu import SHLightsMenu

class SHTreeComboBox(QtGui.QComboBox):
  updateList = QtCore.Signal()

  def __init__(self): 
    super(SHTreeComboBox, self).__init__()

  def showPopup(self):
    self.updateList.emit()
    super(SHTreeComboBox, self).showPopup()
 

class SHTreeViewWidget(QtGui.QWidget):
  sceneHierarchyChanged = QtCore.Signal()
  sceneUpdated = QtCore.Signal(SceneHub.SHGLScene)
  
  def __init__(self, parent, klFile):
    self.parentApp = parent

    super(SHTreeViewWidget, self).__init__()
    self.shGLScene = SceneHub.SHGLScene(self.parentApp.client, klFile)
    #self.controller = controller

    self.treeModel = None
    self.bUpdatingSelectionFrom3D = False
    #self.shGLScene = SceneHub.SHGLScene(self.client)

    self.comboBox = SHTreeComboBox()
    self.shTreeView = SHTreeView(self.parentApp.client, self.shGLScene)
    
    layout = QtGui.QVBoxLayout()
    layout.addWidget(self.comboBox)
    layout.addWidget(self.shTreeView)
    self.setLayout(layout)
    self.comboBox.addItem("Main Scene")

    self.shTreeView.itemSelected.connect(self.onTreeItemSelected)
    self.shTreeView.itemDeselected.connect(self.onTreeItemDeselected)
    self.comboBox.updateList.connect(self.onUpdateSceneList)
    self.comboBox.currentIndexChanged.connect(self.onUpdateScene)
 
    self.onUpdateScene()

  def initMenu(self, menuBar):
    self.assetMenu = SHAssetsMenu(self.shGLScene, "Assets")
    self.lightsMenu = SHLightsMenu(self.shGLScene, "lights")
    menus = menuBar.findChildren(QtGui.QMenu)    
    for menu in menus:      
      if menu.title() == "&File":
        actions = menu.findChildren(QtGui.QAction)
        for action in actions:
          if action.text() == "Quit":
            menu.insertMenu(action, self.assetMenu)
            menu.insertMenu(action, self.lightsMenu)

  def getScene(self):
    return self.shGLScene

  def __resetTree(self):
    self.shTreeView.reset()
    if self.treeModel is not None: self.treeModel = None
 
  def __constructTree(self):
    self.__resetTree()
    self.treeModel = SceneHub.SHTreeModel(self.shGLScene, self.shTreeView)
    self.sceneHierarchyChanged.connect(self.treeModel.onSceneHierarchyChanged)
    self.treeModel.sceneHierarchyChanged.connect(self.onSceneHierarchyChanged)
    sceneRootIndex = self.treeModel.addRootItemsFromScene(self.shGLScene)
    self.shTreeView.setModel(self.treeModel)
    self.shTreeView.setExpanded(sceneRootIndex, True)

  def onUpdateScene(self):
    if not self.shGLScene.hasSG(): return

    sceneName = self.comboBox.currentText()
    if str(sceneName) == "Main Scene":
      #self.shGLScene.setSHGLScene(self.shGLScene.getSHGLScene())
      self.__constructTree()
      #self.sceneUpdated.emit(self.shGLScene)
    
    #elif self.controller.getBinding().getExec().hasVar(str(sceneName)):
      #self.shGLScene.setSHGLScene(self.controller.getBinding().getExec().getVarValue(str(sceneName)))
      #self.__constructTree()
      #self.sceneUpdated.emit(self.shGLScene)
    
    else:
      self.comboBox.clear()
      self.__resetTree()
    
  def onUpdateSceneList(self):
    pass
    '''
      self.comboBox.clear()

      FabricCore::DFGStringResult json =  self.controller.getBinding().getVars()
      FTL::JSONStrWithLoc jsonStrWithLoc( json.getCString() )
      FTL::OwnedPtr<FTL::JSONObject> jsonObject(FTL::JSONValue::Decode( jsonStrWithLoc ).cast<FTL::JSONObject>() )

      std::vector<FTL::JSONObject const *> objects
      objects.push_back(jsonObject.get())
      QStringList sceneNameList

      for(size_t i=0 i<objects.size() i++)
      {
        FTL::JSONObject const * varsObject = objects[i].maybeGetObject( FTL_STR("vars") )
        if(varsObject)
        {
          for(FTL::JSONObject::const_iterator it = varsObject.begin() it != varsObject.end() it++)
          {
            QString sceneName(it.first.c_str())
            FTL::JSONObject const *value = it.second.cast<FTL::JSONObject>()
            for(FTL::JSONObject::const_iterator jt = value.begin() jt != value.end() jt++) 
            {
              if(QString(jt.second.getStringValue().c_str()) == "SHGLScene")
              {
                if(!sceneNameList.contains(sceneName))
                  sceneNameList.append(sceneName)
              }
            }
          }
        }
    
        if(sceneNameList.size() == 0 && !self.shGLScene)
        self.__resetTree()

      if(self.shGLScene) self.comboBox.addItem("Main Scene")
      for(int i=0 i<sceneNameList.size() ++i)
        self.comboBox.addItem(sceneNameList[i])
      }
    '''

  def expandTree(self, level):
    if(level == -1): self.shTreeView.expandAll()
    elif(level>0): self.shTreeView.expandToDepth(level-1)

  def onSceneHierarchyChanged(self):
    #Check if it actually changed, to reduce number of notifications
    if self.shGLScene.hasSG() and self.shGLScene.sceneHierarchyChanged():
      self.sceneHierarchyChanged.emit()

  def onTreeItemSelected(self, item):
    if self.shGLScene.hasSG() and not self.bUpdatingSelectionFrom3D:
      self.shGLScene.treeItemSelected(item)
      self.sceneHierarchyChanged.emit()
     
  def onTreeItemDeselected(self, item):
    if self.shGLScene.hasSG() and not self.bUpdatingSelectionFrom3D:
      self.shGLScene.treeItemDeselected(item) 
      self.sceneHierarchyChanged.emit()

  def onUpdateFrom3DSelection(self):
    if self.shGLScene.hasSG():
      #Ensure there's not a loopback from TreeView selection change to 3D view
      self.bUpdatingSelectionFrom3D = True
      #Ensure it really changed we will be called for any accepted event only a few will be selection changes
      if self.shGLScene.selectionChangedFromManips():
        self.shTreeView.setSelectedObjects(self.shGLScene)
        self.sceneHierarchyChanged.emit()
      self.bUpdatingSelectionFrom3D = False
  