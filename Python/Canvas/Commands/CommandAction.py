#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions
from FabricEngine.Canvas.Commands.CommandManager import *
 
class CommandAction(QtGui.QAction):
    """ CommandAction associates a command to 
        an action, both have the same name.
        The action has
    """

    def __init__(self, parent, cmdName, shortcut, tooltip):
        """ Initializes a CommandAction.
            Arguments:
            - parent: A reference to the parent
            - cmdName: Name of the command
            - shortcut: Shortcut triggering the command
            - tooltip: Action's tooltip
        """

        super(CommandAction, self).__init__(parent)  
         
        self.setText(cmdName)
        self.setToolTip(tooltip)
        self.triggered.connect(self.onTriggered)
        self.setShortcutContext(QtCore.Qt.ApplicationShortcut)

        # Register the action so an item
        # is created in the HotkeyTable.
        Actions.ActionRegistry.GetActionRegistry().registerAction(cmdName, self)

    def onTriggered(self):
        """ \internal.
            Implementation of Actions.BaseAction.
            Create the command.
        """
        try:
            GetCommandManager().createCommand(self.text())
        except Exception as e:    
            print str(e)
