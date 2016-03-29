import optparse, os, sys, math, copy
from FabricEngine import Core, FabricUI
from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine.FabricUI import *
from CanvasWindow import CanvasWindow
 
class SHTreeViewWidget(QtGui.QWidget):
  sceneUpdated = QtCore.Signal(SceneHub.SHGLScene)
  sceneHierarchyChanged = QtCore.Signal()
  
  def __init__(self, client, controller):
    super(SHTreeViewWidget, self).__init__()
    self.client = client
    self.controller = controller

    self.bUpdatingSelectionFrom3D = False
    self.mainSHGLScene = None
    self.treeModel = None
    self.shGLScene = SceneHub.SHGLScene(self.client)

    self.comboBox = SceneHub.SHTreeComboBox()
    self.shTreeView = SceneHub.SHTreeView(self.client)
    
    layout = QtGui.QVBoxLayout()
    layout.addWidget(self.comboBox)
    #layout.addWidget(self.shTreeView)
    self.setLayout(layout)

    #self.shTreeView.itemSelected.connect(self.onTreeItemSelected)
    #self.shTreeView.itemDeselected.connect(self.onTreeItemDeselected)
    #self.comboBox.updateList.connect(self.onUpdateSceneList)
    #self.comboBox.currentIndexChanged.connect(self.onUpdateScene)
 
  def setMainScene(self, mainScene):
    print "setMainScene"
    #self.shGLScene = mainScene
    self.shGLScene.setSHGLScene(mainScene.getSHGLScene())
    #sg = mainScene.getSG()
    #self.sg = copy.deepcopy(mainScene.getSG())

    #self.comboBox.addItem("Main Scene")
    self.__constructTree()

  def getScene(self):
    return self.shGLScene

  def __resetTree(self):
    pass
    #self.shTreeView.reset()
    #if self.treeModel is not None:
      #delete self.treeModel
      #self.treeModel = None
 
  def __constructTree(self):
    if self.shGLScene is None: return
    self.__resetTree()

    sg = self.shGLScene.getSG()
    self.treeModel = SceneHub.SHTreeModel(1, self.client, self.shTreeView)
    #3self.sceneHierarchyChanged.connect(self.treeModel.onSceneHierarchyChanged)
    #self.treeModel.sceneHierarchyChanged.connect(self.onSceneHierarchyChanged)

    #sceneRootIndex = self.treeModel.addRootItem(self.shGLScene.getSceneRoot())
    #self.treeModel.addRootItem(self.shGLScene.getAssetLibraryRoot())
    #self.treeModel.addRootItem(self.shGLScene.getMaterialLibraryRoot())
    #self.treeModel.addRootItem(self.shGLScene.getImageLibraryRoot())

    #self.shTreeView.setModel(self.treeModel)
    #self.shTreeView.setExpanded(selfceneRootIndex, True)

  def onUpdateScene(self):
    sceneName = self.comboBox.currentText()
    print "onUpdateScene " + str(sceneName)
    if str(sceneName) == "Main Scene":
      #self.shGLScene.setSHGLScene(self.mainSHGLScene.getSHGLScene())
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
    
        if(sceneNameList.size() == 0 && !self.mainSHGLScene)
        self.__resetTree()

      if(self.mainSHGLScene) self.comboBox.addItem("Main Scene")
      for(int i=0 i<sceneNameList.size() ++i)
        self.comboBox.addItem(sceneNameList[i])
      }
    '''

  def expandTree(self, level):
    if(level == -1): self.shTreeView.expandAll()
    elif(level>0): self.shTreeView.expandToDepth(level-1)

  def onSceneHierarchyChanged(self):
    pass
    #Check if it actually changed, to reduce number of notifications
    #if self.shGLScene is not None :
    #33333  if self.shGLScene.sceneHierarchyChanged():
        #self.sceneHierarchyChanged.emit()

  def onTreeItemSelected(self, item):
    pass
    #if self.shGLScene is not None:
    #  if self.bUpdatingSelectionFrom3D == False :
        #self.shGLScene.treeItemSelected(item.getSGObject())
        #self.sceneHierarchyChanged.emit()
     
  def onTreeItemDeselected(self, item):
    pass
    #if self.shGLScene is not None:
    #  if self.bUpdatingSelectionFrom3D == False : 
        #self.shGLScene.treeItemDeselected(item.getSGObject()) 
        #self.sceneHierarchyChanged.emit()

  def onUpdateFrom3DSelection(self):
    pass
    #if self.shGLScene is not None:
    #  #Ensure there's not a loopback from TreeView selection change to 3D view
      #self.bUpdatingSelectionFrom3D = True
      #Ensure it really changed we will be called for any accepted event only a few will be selection changes
      #if self.shGLScene.selectionChangedFromManips():
        #self.shTreeView.setSelectedObjects(self.shGLScene.getSelectedObjects())
        #self.sceneHierarchyChanged.emit()
      #self.bUpdatingSelectionFrom3D = False
  