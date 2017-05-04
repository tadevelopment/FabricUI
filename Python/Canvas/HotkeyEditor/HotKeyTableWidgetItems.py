#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions, Util
 
class TableWidgetItemColor:
    Disabled = QtCore.Qt.gray
    IsAction = QtGui.QColor(204, 102, 0, 255)
    IsCppCommand = QtGui.QColor(0, 191, 191, 255)
    IsKLCommand = QtGui.QColor(0, 102, 204, 255)
    IsPythonCommand = QtGui.QColor(39, 168, 223 , 255)
 
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
        
        self.widgetItemColor = TableWidgetItemColor
        if not isEditable:
            font = self.font()
            self.setFont(font)
            self.setForeground(self.widgetItemColor.Disabled)
 
class ActionTableWidgetItem(BaseTableWidgetItem):
    """ ActionTableWidgetItem specializes BaseTableWidgetItem
        to update the item tooltip according to the action's one.
    """
    
    def __init__(self, actionName, tooltip, isEditable, cmdImplType = None):
        """ Initializes the ActionTableWidgetItem.
        
            Arguments:
            - actionName: The name of the action
            - tooltip: The action tool tip
            - isEditable: If true, the item is editable
        """

        super(ActionTableWidgetItem, self).__init__(
            actionName, 
            isEditable, 
            )
        
        self.actionName = actionName
        self.setToolTip(tooltip)
        self.setFlags(QtCore.Qt.NoItemFlags)
        
        if cmdImplType:
            if cmdImplType == "CPP":
                color = self.widgetItemColor.IsCppCommand

            elif cmdImplType == "KL":
                color = self.widgetItemColor.IsKLCommand

            elif cmdImplType == "PYTHON":
                color = self.widgetItemColor.IsPythonCommand

        else:
            color = self.widgetItemColor.IsAction

        self.__setIcon(color)

    def __setIcon(self, color):
      
        img = QtGui.QImage(20, 20, QtGui.QImage.Format_ARGB32)
        img.fill(QtGui.QColor(0, 0, 0, 0))

        for i in range(0, 20):
            for j in range(8, 12):
                img.setPixel(
                    j, 
                    i, 
                    color.rgba()
                    )

        # https://forum.qt.io/topic/6971/solved-qicon-disabled-full-color/7
        icon = QtGui.QIcon()
        icon.addPixmap(
            QtGui.QPixmap.fromImage(img),
            QtGui.QIcon.Disabled
            )

        self.setIcon(icon)

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
        
        super(ShorcutTableWidgetItem, self).__init__(
            shorcut, 
            isEditable
            )

        if not isEditable:
            self.setFlags(QtCore.Qt.NoItemFlags)

        if isGlobal:
            font = self.font()
            font.setBold(True)
            self.setFont(font)
 