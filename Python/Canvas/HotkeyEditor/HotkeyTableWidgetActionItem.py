#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions
 
class HotkeyTableWidgetActionItem(QtGui.QTableWidgetItem):
    """ HotkeyTableWidgetActionItem specilized QTableWidgetItem
        to update the item tooltip according to the action's one.
    """
    
    def __init__(self, actionName, tooltip):
        """ Initializes the HotkeyTableWidgetActionItem.
        
            Arguments:
            - actionName: The name of the action
            - tooltip: The action tool tip
        """
        
        super(HotkeyTableWidgetActionItem, self).__init__(actionName)
        self.actionName = actionName
        self.setToolTip(tooltip)
        self.setFlags(QtCore.Qt.NoItemFlags)

    def onActionChanged(self):
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        action = actionRegistry.getAction(self.actionName)
        if action:
            tooltip = action.toolTip()
            if not tooltip:
                tooltip = action.text()
            self.setToolTip(tooltip)
