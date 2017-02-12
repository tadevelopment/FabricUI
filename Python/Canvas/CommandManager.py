#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine import FabricUI
from FabricEngine.FabricUI import Commands
from FabricEngine.Canvas.CommandRegistery import CommandRegistery
 
class CommandManager(Commands.BaseCommandManager_Python):
    
    """ CommandManager specializes BaseCommandManager.
        See Commands/BaseCommandManager.h(cpp) and Commands/BaseCommandManager_Python.h(cpp).
        
        To register a command: CommandRegistery.RegisterCommand(cmdName, userData)
        To create a command: cmd = cmdManager.createCommand(cmdName)
    """

    def __init__(self):
        super(CommandManager, self).__init__()
        
        # \internal
        # There is no "new" is python, so we need to own the commands created in Python.
        # They are refered by the BaseCommandManager undo-redo stacks. 
        self.__flatCommandsStack = []

    def createCommand(self, cmdName, args = {}, doCmd = True):
        """ Implementation of BaseCommandManager.
            Raises an exception if an error occurs. 
        """
        try:
            cmd = CommandRegistery.CreateCommand(cmdName)
            
            # In case the command has been registered in C++ but
            # is created from Python, ask the C++ command registery to do it.
            if cmd is None:
                cmd = super(CommandManager, self).createCommand(cmdName)

            cmd.creationCallback(self)
            
            if len(args) > 0:
              self.checkCommandArgs(cmd, args)

            if doCmd :
              self.doCommand(cmd)

            return cmd

        except Exception as e:    
            raise Exception(e)

    def doCommand(self, cmd):
        """ Implementation of BaseCommandManager.
            Raises an exception if an error occurs. 
        """
        error = super(CommandManager, self).doCommand_Python(cmd)
        if error:
            raise Exception(error)

    def undoCommand(self):
        """ Implementation of BaseCommandManager.
            Raises an exception if an error occurs. 
        """
        error = super(CommandManager, self).undoCommand_Python()
        if error:
            raise Exception(error)

    def redoCommand(self):
        """ Implementation of BaseCommandManager.
            Raises an exception if an error occurs. 
        """
        error = self.redoCommand_Python()
        if error:
            raise Exception(error)

    def clear(self):
        """ Implementation of BaseCommandManager
        """
        super(CommandManager, self).clear()
        self.__flatCommandsStack = []

    def checkCommandArgs(self, cmd, args):
        """ \internal
            Implementation of BaseCommandManager.
            Raises an exception if an error occurs. 
        """
        error = self.checkCommandArgs_Python(cmd, args)
        if error:
            raise Exception(error)

    def clearRedoStack(self):
        """ \internal 
            Implementation of BaseCommandManager
        """
        count = self.totalCountAtStackIndex()
        super(CommandManager, self).clearRedoStack()
        self.__flatCommandsStack = self.__flatCommandsStack[:count]

    def commandPushedCallback(self, cmd):
        """ \internal 
            Implementation of BaseCommandManager
        """
        self.__flatCommandsStack.append(cmd)
