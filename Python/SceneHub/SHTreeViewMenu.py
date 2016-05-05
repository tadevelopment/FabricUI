
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.FabricUI import Menus

class SHTreeViewMenu(Menus.BaseMenu):

    """SHTreeViewMenu

    SHAssetsMenu specializes BaseMenu to edit the SceneHub.SHTreeView items properties.
    It's used to show/hides the properties/generators under the SGObjects.

    Arguments:
        shTreeViewsManager (SceneHub.SHTreeViewsManager): A reference to a SHTreeViewsManager.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, shTreeViewsManager, parent = None):
        self.shTreeViewsManager = shTreeViewsManager
        super(SHTreeViewMenu, self).__init__(self.shTreeViewsManager.client, "Tree View", parent)
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

        self.shTreeViewsManager.setShowProperties(self.showProperty.isChecked())
        self.shTreeViewsManager.setShowOperators(self.showPropertyGenerator.isChecked())
