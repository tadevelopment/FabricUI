#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands
from FabricEngine.Canvas.CommandRegistry import *
 

class CommandManager(Commands.BaseCommandManager_Python):
    
    """ CommandManager specializes BaseCommandManager (C++). 
        See Commands/BaseCommandManager.h(cpp) and Commands/BaseCommandManager_Python.h(cpp).
        
        The registery is shared between the C++ and Python, so commands defined in Python 
        can be created from C++ code too, and vice versa.

        The CommandManager is a singleton and should not be created directly.
        - Get the manager: cmdManager = GetCommandManager()

        - Register a command: GetCommandRegistry().registerComand(cmdName, userData)
        
        - Create a command (C++/Python): cmd = cmdManager.createCommand(cmdName)
    """
    
    def __init__(self):
        super(CommandManager, self).__init__()
        
        # !!! Very important !!!!!
        # Here, we set the C++ pointer of the C++ CommandManager 
        # singleton equal to this python instance of CommandManager.
        Commands.BaseCommandManager.SetCommandManagerSingleton(self)

        # There is no "new" is python, so we need to own the commands created in Python.
        # They are refered by the BaseCommandManager undo-redo stacks. 
        self.__flatCommandsStack = []
 
    def createCommand(self, cmdName, args = {}, doCmd = True):
        """ Implementation of BaseCommandManager.
            Raises an exception if an error occurs. 
        """
        try:
            cmd = GetCommandRegistry().createCommand(cmdName)
                  
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
        error = super(CommandManager, self).redoCommand_Python()
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
        error = super(CommandManager, self).checkCommandArgs_Python(cmd, args)
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


# \internal
# !!!!! Store the reference to the 
# CommandManager singleton !!!!
s_cmdManager = CommandManager()

def GetCommandManager():
    """ Gets the CommandManager singleton.
    """
    return Commands.BaseCommandManager.GetCommandManager()