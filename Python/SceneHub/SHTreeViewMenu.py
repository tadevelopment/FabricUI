
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.Canvas.BaseMenu import BaseMenu

class SHTreeViewMenu(BaseMenu):

    """SHTreeViewMenu

    SHAssetsMenu specializes BaseMenu to edit the SceneHub.SHTreeView items properries.
    For example, it's used to show/hides the properties/generators under the tree items

    Arguments:
        shTreeView (SceneHub.SHTreeView): A reference to a SHTreeView.
        parent (QWidget): The menu widget parent, can be None.

    """

    def __init__(self, shTreeView, parent = None):
        self.shTreeView = shTreeView
        super(SHTreeViewMenu, self).__init__("Tree View", parent)
     
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

        self.shTreeView.setShowProperties(self.showProperty.isChecked())
        self.shTreeView.setShowOperators(self.showPropertyGenerator.isChecked())
