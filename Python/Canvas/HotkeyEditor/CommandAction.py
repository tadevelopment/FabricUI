#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions
from FabricEngine.Canvas.Commands.CommandManager import *
 
class CommandAction(Actions.BaseAction):
    """ CommandAction associates a command to 
        an action, both have the same name.
        The action has
    """

    def __init__(self, parent, cmdName, implType, shortcut, tooltip, isScriptable):
        """ Initializes a CommandAction.
            Arguments:
            - parent: A reference to the parent
            - cmdName: Name of the command
            - shortcut: Shortcut triggering the command
            - tooltip: Action's tooltip
        """
        super(CommandAction, self).__init__(parent)
        
        self.implType = implType
        self.cmdName = cmdName

        super(CommandAction, self).init(
            cmdName, 
            tooltip, 
            shortcut, 
            QtCore.Qt.ApplicationShortcut,
            True,
            not isScriptable)  
 
    def onTriggered(self):
        """ \internal.
            Implementation of Actions.BaseAction.
            Create the command.
        """
        try:
            GetCommandManager().createCommand(self.cmdName)
            GetCommandManager().synchronizeKL()

        except Exception as e:    
            print str(e)
