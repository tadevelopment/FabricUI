
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
from FabricEngine.SceneHub.SHBaseMenu import SHBaseMenu

class SHTreeViewMenu(SHBaseMenu):

  def __init__(self, treeViewWidget, title = None, parent = None):
    self.shTreeViewWidget = treeViewWidget
    super(SHTreeViewMenu, self).__init__(None, "Tree View", parent)
 
  def constructMenu(self):
    self.showProperty = self.addAction("Show properties")
    self.showProperty.setCheckable( True )
    self.showProperty.setChecked( True )
    
    self.showPropertyGenerator = self.addAction("Show property generators")
    self.showPropertyGenerator.setCheckable( True )
    self.showPropertyGenerator.setChecked( True )

    self.showProperty.triggered.connect(self.updateTreeViewVisibility)
    self.showPropertyGenerator.triggered.connect(self.updateTreeViewVisibility)
 
  def updateTreeViewVisibility(self):   
    self.shTreeViewWidget.setShowProperties(self.showProperty.isChecked())
    self.shTreeViewWidget.setShowOperators(self.showPropertyGenerator.isChecked())
