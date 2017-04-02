#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions, Util
 
class BaseTableWidgetItem(QtGui.QTableWidgetItem):
    """ BaseTableWidgetItem specializes QTableWidgetItem.
    """
    
    def __init__(self, text, isEditable):
        """ Initializes the BaseTableWidgetItem.
        
            Arguments:
            - text: The name of the item
            - isEditable: If true, the item is editable
        """
        
        super(BaseTableWidgetItem, self).__init__(text)
 
        if not isEditable:
            font = self.font()
            font.setItalic(True)
            self.setFont(font)
            self.setForeground(QtCore.Qt.gray)
 
class ActionTableWidgetItem(BaseTableWidgetItem):
    """ ActionTableWidgetItem specializes BaseTableWidgetItem
        to update the item tooltip according to the action's one.
    """
    
    def __init__(self, actionName, tooltip, isEditable):
        """ Initializes the ActionTableWidgetItem.
        
            Arguments:
            - actionName: The name of the action
            - tooltip: The action tool tip
            - isEditable: If true, the item is editable
        """
        
        super(ActionTableWidgetItem, self).__init__(actionName, isEditable)
        self.actionName = actionName
        self.setToolTip(tooltip)
        self.setFlags(QtCore.Qt.NoItemFlags)
        self.setIcon( Util.LoadIcon.load( "actionIcon.png" ).scaledToWidth( 20, QtCore.Qt.SmoothTransformation ) )

    def onActionChanged(self):
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        action = actionRegistry.getAction(self.actionName)
        if action:
            tooltip = action.toolTip()
            if not tooltip:
                tooltip = action.text()
            self.setToolTip(tooltip)

class ShorcutTableWidgetItem(BaseTableWidgetItem):
    """ ShorcutTableWidgetItem specializes BaseTableWidgetItem.
    """
    
    def __init__(self, shorcut, isEditable, isGlobal):
        """ Initializes the ShorcutTableWidgetItem.
        
            Arguments:
            - shorcut: The action shortcut
            - isEditable: If true, the item is editable
        """
        
        super(ShorcutTableWidgetItem, self).__init__(shorcut, isEditable)
        if not isEditable:
            self.setFlags(QtCore.Qt.NoItemFlags)

        if isGlobal:
            font = self.font()
            font.setBold(True)
            self.setFont(font)
 