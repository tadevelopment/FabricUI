#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.Canvas.Utils import *
from FabricEngine.FabricUI import Commands as CppCommands
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.Commands.CommandArgHelpers import CommandArgHelpers

class CommandManager(CppCommands.KLCommandManager_Python):
    
    """ CommandManager specializes KLCommandManager_Python (C++). See Commands/CommandManager.h(cpp),
        Commands/KLCommandManager.h(cpp) and Commands/KLCommandManager_Python.h(cpp). The manager is 
        shared between the C++ and Python, so commands defined in Python can be created from C++ code, 
        and vice versa.

        The CommandManager is a singleton and should not be created directly.
        - Create the singleton: cmdManager = CommandManager(fabricClient)

        - Get the singleton: cmdManager = GetCmdManager()

        Available methods:
        - Create a command (KL/C++/Python): cmd = cmdManager.createCmd(cmdName, args, doIt)
                
        - Execute a command: cmdManager.doCmd(cmd)

        - Process undo: cmdManager.undoCmd()

        - Process redo: cmdManager.redoCmd()

        - Clear the stacks: cmdManagerclear()

        - Number of commands (undo+redo): cmdManager.count()

        - Get current stack index: cmdManager.getStackIndex()
        
        - Get the stack content as string: cmdManager.getContent()

        - Synchronize with the KL manager: cmdManager.synchronizeKL()


        When a command is registered, a function creating the command with named arguments is dynimi-
        cally defined in Python. Each function in added to the python 'Commands' module. For example, 
        the scriptable command FooCmd with arguments {arg_1:"foo_1", arg_2:"foo_2"} can be created  
        and executed via:
        - from FabricEngine.Canvas.Commands.CommandManager import *
        - import Commands
        - Commands.FooCmd(arg_1 = "foo_1", arg_2 = "foo_2)

        This is equivalent to (see CommandManager.py):
        - args = {arg_1:"foo_1", arg_2:"foo_2"}
        - GetCmdManager().createCommand("FooCmd", args, True)
    """
    
    def __init__(self):
        """ Initializes the CommandManager.
        """
        super(CommandManager, self).__init__()
        # There is no "new" in python, we need to own the commands created
        # in Python. They are referenced in the C++ CommandManager stacks. 
        self.__flatCommandsStack = []

        # Connect our-self.
        cmdRegistry = CreateCmdRegistry()
        cmdRegistry.commandRegistered.connect(self.__onCommandRegistered)
    
    def createCmd(self, cmdName, args = {}, doCmd = True):
        """ Creates and executes a command (if doCmd == true).
            If executed, the command is added to the manager stack.
            Raises an exception if an error occurs.
        """
        try:
            cmd = GetCmdRegistry().createCmd(cmdName)
            if len(args) > 0:
                createRTValCommand, args = CommandArgHelpers.CastAndCheckCmdArgs(cmd, args)
                if createRTValCommand:
                    self._checkRTValCommandArgs_Python(cmd, args)
                else:
                    self._checkCommandArgs_Python(cmd, args)
               
            if doCmd:
                self.doCmd(cmd)
            return cmd
        except Exception as e:   
            raise Exception(e)

    def doCmd(self, cmd):
        """ Executes a command and adds it to the undo stack.
            Raises an exception if an error occurs.
        """
        return self._doCommand_Python(cmd)

    def undoCmd(self):
        """ Undoes the current command.
            Raises an exception if an error occurs. 
        """
        self._undoCommand_Python()
        
    def redoCmd(self):
        """ Redoes the next command.
            Raises an exception if an error occurs. 
        """
        self._redoCommand_Python()

    def clear(self):
        """ Clears all the commands.
        """
        super(CommandManager, self).clear()
        self.__flatCommandsStack = []

    def getCmdAtIndex(self, index):
        """ Gets the command at index 'index'.
            Returns none if the index is out of ranges.
        """
        error, cmd = self._getCommandAtIndex_Python(index)
        return cmd

    def synchronizeKL(self):
        """ Implementation of Commands.KLCommandManager
        """
        self._synchronizeKL_Python()

    ### \internal, don't call these methods.
    def _createCommand_Python(self, cmdName, args = {}, doCmd = True):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        try:
            cmd = GetCmdRegistry().createCmd(cmdName)
            if len(args) > 0:
                self._checkCommandArgs_Python(cmd, CommandArgHelpers.CastCmdArgsToRTVal(cmd, args))
            if doCmd:
                self._doCommand_Python(cmd)
            return cmd
        except Exception as e:    
            raise Exception(e)

    def _checkCommandArgs_Python(self, cmd, args):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        error = super(CommandManager, self)._checkCommandArgs_Python(cmd, args)
        if error:
            raise Exception(error)
        return error

    def _createRTValCommand_Python(self, cmdName, args = {}, doCmd = True):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        try:
            cmd = GetCmdRegistry().createCmd(cmdName)
            if len(args) > 0:
                self._checkRTValCommandArgs_Python(cmd, args)
            if doCmd:
                self._doCommand_Python(cmd)
            return cmd
        except Exception as e:    
            raise Exception(e)

    def _checkRTValCommandArgs_Python(self, cmd, args):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        error = super(CommandManager, self)._checkRTValCommandArgs_Python(
            cmd, args)
        if error:
            raise Exception(error)
        return error

    def _doCommand_Python(self, cmd):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        error = super(CommandManager, self)._doCommand_Python(cmd)
        if error:
            raise Exception(error)
        return error

    def _undoCommand_Python(self):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        error = super(CommandManager, self)._undoCommand_Python()
        if error:
            raise Exception(error)
        return error

    def _redoCommand_Python(self):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        error = super(CommandManager, self)._redoCommand_Python()
        if error:
            raise Exception(error)
        return error

    def _getCommandAtIndex_Python(self, index):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        error, cmd = super(CommandManager, self)._getCommandAtIndex_Python()
        if cmd is None:
            raise Exception(error)
        return error, cmd

    def clearRedoStack(self):
        """ \internal, impl. of Commands.KLCommandManager_Python. """
        count = self._totalCountAtStackIndex()
        super(CommandManager, self).clearRedoStack()
        self.__flatCommandsStack = self.__flatCommandsStack[:count]

    def _commandPushed_Python(self, cmd, isLowCmd):
        """ \internal, impl. of Commands.KLCommandManager_Python. 
            Add the commands to the python stack.
        """
        self.__flatCommandsStack.append(cmd)
    
    def _synchronizeKL_Python(self):
        """ \internal, impl. of Commands.KLCommandRegistry_Python. """
        error = super(CommandManager, self)._synchronizeKL_Python()
        if error:
            raise Exception(error)
        return error
     
    def __onCommandRegistered(self, cmdName, cmdType, implType):
        """ \internal, callback when a command is registered.
            Run-time creation of the function to create the command.
            The function is added to the "Commands" module. 
        """
        exec('\
def ' + cmdName + '(**kwargs):\n\
    try:\n\
        return GetCmdManager().createCmd("' + cmdName + '", kwargs )\n\
    except Exception as e:\n\
        raise Exception(e)\n\
setattr(GetOrCreateModule("Commands") , "' + cmdName + '", '+ cmdName + ')')


# \internal, Create the "Commands" module right now so it can  
# be imported with the usual syntax later : 'import Commands'
GetOrCreateModule('Commands')

# \internal, store the manager singleton
global s_cmdManagerSingleton
s_cmdManagerSingleton = None

def GetCmdManager():
    return CreateCmdManager()

def CreateCmdManager():
    """ Creates the CommandManager singleton.
    """
    global s_cmdManagerSingleton
    if s_cmdManagerSingleton is None:
        # Be sure the command registry is created.
        s_cmdManagerSingleton = CommandManager()
    return s_cmdManagerSingleton
