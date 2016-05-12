from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI, Util, CAPI
from FabricEngine.FabricUI import *
from FabricEngine.Util import *
from FabricEngine.CAPI import *
from FabricEngine.SceneHub.SHTreeView import SHTreeView


class SHTreeViewsManager(FabricUI.SceneHub.SHTreeViewsManager):

    """SHTreeViewsManager

    SHTreeViewsManager owns a SHTreeView and allows to display 
    the ojects/properties of a SHGLScene.
    
    In SceneHub, the app owns a SHGLScene by default (the main scene).
    However, other SHGLScenes can be defined within the canvas graph as canvas nodes.
    A SHTreeComboBox is used to display all the available scenes in the app and switch among them.
    The treeView is then updated to display the objects of the selected scene.

    The class gives also access to the base signals/slots so it can be specialized if needed.

    Arguments:
        client (FabricEngine.Core.Client): A reference to the FabricCore.Client.
        dfgWidget (DFG.DFGWidget): A reference to the DFGWidget.
        shStates (SceneHub.SHStates): A reference to the SHStates.
        shMainGLScene (SceneHub.SHGLScene): A reference to a main SHGLScene (Can be None).
    """

    def __init__(self, client, dfgWidget, shStates, shMainGLScene = None):
        self.client = client
        super(SHTreeViewsManager, self).__init__(client, dfgWidget, shStates)
 
        # Shows the main-scene (owns by the application) in the treeView.
        self.shMainGLScene = shMainGLScene
        self.m_comboBox.addItem("Main Scene")
        self.onConstructScene("Main Scene")

    def onConstructScene(self, sceneName):
        """ Constructs the TreeView from the selected scene,
            the main scene, or from a scene defined in a DFG node.
        """

        if self.shMainGLScene is not None and str(sceneName) == "Main Scene":
            self.m_shGLScene.setSHGLScene(self.shMainGLScene)
            self.constructTree()
            self.activeSceneChanged.emit(self.m_shGLScene)
        
        elif self.dfgWidget.getDFGController().getBinding().getExec().hasVar(str(sceneName)):
            self.m_shGLScene.setSHGLScene(self.dfgWidget.getDFGController().getBinding(), sceneName)
            self.constructTree()
            self.activeSceneChanged.emit(self.m_shGLScene)
        
        else:
            self.m_comboBox.clear()
            self.resetTree()
    
    def onUpdateSceneList(self):
        """ Finds all the SHGLScenes (in the app and in the canvas graph)
            and populates the combobox with their name.
        """

        self.m_comboBox.clear()
        binding = self.dfgWidget.getDFGController().getBinding()

        typeList = []
        typeList.append('SHGLScene')
        sceneNameList = FabricUI.DFG.DFGController.getVariableWordsFromBinding(binding, '.', typeList)

        if len(sceneNameList) == 0 and not self.m_shGLScene.hasSG(): 
            self.resetTree()

        if self.shMainGLScene is not None and self.shMainGLScene.hasSG(): 
            self.m_comboBox.addItem("Main Scene")

        for sceneName in sceneNameList: 
            self.m_comboBox.addItem(sceneName)
