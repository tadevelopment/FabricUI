
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.FabricUI import Menus

class SHTreeViewMenu(Menus.BaseMenu):

    """SHTreeViewMenu

    SHAssetsMenu specializes BaseMenu to edit the SceneHub.SHTreeView items properties.
    It's used to show/hides the properties/generators under the SGObjects.

    Arguments:
        SHTreeViewsManager (SceneHub.SHTreeViewsManager): A reference to a SHTreeViewsManager.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, SHTreeViewsManager, parent = None):
        self.SHTreeViewsManager = SHTreeViewsManager
        super(SHTreeViewMenu, self).__init__(self.SHTreeViewsManager.m_client, "Tree View", parent)
        self.constructMenu()

    def constructMenu(self):
        """Implementation of BaseMenu.
        """

        self.showProperty = self.addAction("Show properties")
        self.showProperty.setCheckable( True )
        self.showProperty.setChecked( True )
        
        self.showPropertyGenerator = self.addAction("Show property generators")
        self.showPropertyGenerator.setCheckable( True )
        self.showPropertyGenerator.setChecked( True )

        self.showProperty.triggered.connect(self.updateTreeViewVisibility)
        self.showPropertyGenerator.triggered.connect(self.updateTreeViewVisibility)
     
    def updateTreeViewVisibility(self):   
        """Shows/hides the treeView items properties/generators.
        """

        self.SHTreeViewsManager.setShowProperties(self.showProperty.isChecked())
        self.SHTreeViewsManager.setShowOperators(self.showPropertyGenerator.isChecked())
