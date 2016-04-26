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

    """SHTreeViewsManager

    The SHTreeViewsManager owns a SHTreeView and allows to display 
    the ojects/properties of a specific SHGLScene.
    
    In SceneHub, the app owns a scene by default (the main scene).
    However, other scenes can be defined within the canvas graph as canvas nodes.
    A combobox is used to display all the available scenes in the app and switch among them.
    The treeView is then updated to display the objects of the selected scene.

    The class gives also access to the base signals/slots so it can be specialized if needed.

    Arguments:
        client (FabricEngine.Core.Client): A reference to the FabricCore.Client.
        dfgWidget (DFG.DFGWidget): A reference to the DFGWidget.
        shStates (SceneHub.SHStates): A reference to the SHStates.
        shMainGLScene (SceneHub.SHGLScene): A reference to a main SHGLScene.
    """

    sceneChanged = QtCore.Signal()
    sceneHierarchyChanged = QtCore.Signal()
    itemDoubleClicked = QtCore.Signal(SceneHub.SHTreeItem)
    activeSceneChanged = QtCore.Signal(SceneHub.SHGLScene)
      
    def __init__(self, client, dfgWidget, shStates, shMainGLScene):
        self.dfgWidget = dfgWidget
        self.shStates = shStates
        self.showProperties = True
        self.showOperators = True
        self.client = client
     
        super(SHTreeViewsManager, self).__init__()
        self.shMainGLScene = shMainGLScene
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

        self.comboBox.constructSceneList.connect(self.__onConstructSceneList)
        self.comboBox.currentIndexChanged.connect(self.__onConstructScene)

        self.activeSceneChanged.connect( self.shStates.onActiveSceneChanged )

        self.__onConstructScene()
 
    def getScene(self):
        """ Gets a reference to the current SHGLScene.
        """

        return self.shGLScene

    def __resetTree(self):
        """Resets the TreeView and it's model.
        """

        self.shTreeView.reset()
        if self.treeModel is not None: 
            self.treeModel = None
     
    def __constructTree(self):
        """ Creates the TreeView, resets the model with the new scenegraph.
        """

        self.__resetTree()

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

    def __onConstructScene(self):
        """ Constructs the TreeView from the selected scene :
            the main scene, or from a scene defined in a DFG node.
        """

        sceneName = self.comboBox.currentText()
        if str(sceneName) == "Main Scene":
            self.shGLScene.setSHGLScene(self.shMainGLScene)
            self.__constructTree()
            self.activeSceneChanged.emit(self.shGLScene)
        
        elif self.dfgWidget.getDFGController().getBinding().getExec().hasVar(str(sceneName)):
            self.shGLScene.setSHGLScene(self.dfgWidget.getDFGController().getBinding(), sceneName)
            self.__constructTree()
            self.activeSceneChanged.emit(self.shGLScene)
        
        else:
            self.comboBox.clear()
            self.__resetTree()
    
    def __onConstructSceneList(self):
        """ Finds all the SHGLScenes in the app (defined in the canvas graph too)
            and populates the combobox with their name.
        """

        self.comboBox.clear()
        binding = self.dfgWidget.getDFGController().getBinding()
        sceneNameList = self.shGLScene.getSceneNamesFromBinding(binding)
        if len(sceneNameList) == 0 and not self.shGLScene.hasSG(): 
            self.__resetTree()
        if self.shMainGLScene.hasSG(): 
            self.comboBox.addItem("Main Scene")
        for sceneName in sceneNameList: 
            self.comboBox.addItem(sceneName)
    
    def setShowProperties(self, show):
        """ Shows the SGObject properties in the treeView.
        """

        self.showProperties = show
        if self.treeModel is not None:
            self.treeModel.setShowProperties(show)

    def setShowOperators(self, show):
        """ Shows the SGObject operators in the treeView.
        """

        self.showOperators = show
        if self.treeModel is not None:
            self.treeModel.setShowOperators(show)

    def expandTree(self, level):
        """ Expands the treeView at level 'level'.
        """

        # Out of range unsigned int in KL.
        if level == 4294967295: 
            self.shTreeView.expandAll()
        else: 
            if level > 0: 
                self.shTreeView.expandToDepth(level-1)

    def onSceneHierarchyChanged(self):
        """ Updates the scene (if structural changes).
        """

        self.sceneHierarchyChanged.emit()

    def onSceneChanged(self):
        """ Updates the scene (if treeView selection changed).
        """

        self.sceneChanged.emit()

    def onSelectionCleared(self):
        """ Called when the selection is cleared (from the 3DView).
        """

        if not self.bUpdatingSelection:
            self.shStates.clearSelection()

    def onTreeItemSelected(self, item):
        """ Updates the current selection.
        """

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
        """ Updates the current selection.
        """

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
        """ Updatess the selection properties in the valueEditor.
        """

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
        """ Called when the selection changed (from the 3DView).
        """

        if not self.bUpdatingSelection:
        # Ensure there's not a loopback when the selection is being updated
            self.bUpdatingSelection = True
            self.shTreeView.setSelectedObjects(self.shStates.getSelectedObjects())
            self.bUpdatingSelection = False
