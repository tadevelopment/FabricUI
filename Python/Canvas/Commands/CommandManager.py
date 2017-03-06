#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands
from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.Utils import *

class CommandManager(Commands.CommandManager_Python):
    
    """ CommandManager specializes CommandManager (C++). See Commands/CommandManager.h(cpp) 
        and Commands/CommandManager_Python.h(cpp). The manager is shared between the C++ and 
        Python, so commands defined in Python can be created from C++ code too, and vice versa.

        The CommandManager is a singleton and should not be created directly.
        - Create the registry: cmdManager = CommandManager(fabricClient)

        - Get the manager: cmdManager = GetCommandManager()

        Usage:
        - Create a command (KL/C++/Python): cmd = cmdManager.createCommand(cmdName, args, doIt)
        
        - Execute a command (KL/C++/Python): cmdManager.doCommand(cmd)
        
        - Proces undo: cmdManager.undoCommand()

        - Proces redo: cmdManager.redoCommand()

        - Synchronize with the KL manager: cmdManager.synchronizeKL()

        When a command in registered, a function creating the command with named arguments is dynimically  
        defined in Python. Each function in added to the python 'Commands' module. For example, the scriptable 
        command FooCmd with arguments {arg_1:"foo_1", arg_2:"foo_2"} can be created and executed via:
        - from FabricEngine.Canvas.Commands.CommandRegistry import *
        - import Commands
        - Commands.FooCmd(arg_1 = "foo_1", arg_2 = "foo_2)

        This is equivalent to (see CommandManager.py):
        - args = {arg_1:"foo_1", arg_2:"foo_2"}
        - GetCommandManager().createCommand("FooCmd", args, True)
    """
    
    def __init__(self, client):
        """ Initializes the CommandManager.

            Arguments:
            - client: A FabricCore client.
        """

        super(CommandManager, self).__init__(client)
        # There is no "new" is python, so we need to own the commands created in Python.
        # They are refered by the CommandManager undo-redo stacks. 
        self.__flatCommandsStack = []

    def createCommand(self, cmdName, args = {}, doCmd = True):
        """ Implementation of CommandManager.
            Raises an exception if an error occurs. 
        """

        try:
            # Be sure that all the args are stings.
            self.__checkArgsType(args);

            cmd = GetCommandRegistry().createCommand(cmdName)
                  
            if len(args) > 0:
              self.checkCommandArgs(cmd, args)

            if doCmd :
              self.doCommand(cmd)

            return cmd

        except Exception as e:    
            raise Exception(e)

    def __checkArgsType(self, args):
        """ \internal
            Verify that the args keys and values are strings. 
        """

        for key, value in args.iteritems():

            if not issubclass(type(key), str) and not issubclass(type(key), unicode):
                raise Exception("CommandManager.__checkArgsType, error: arg[" + str(key) + "] " +
                                "must be a string, not a " + str(type(key)) )

            if not issubclass(type(value), str) and not issubclass(type(value), unicode):
                raise Exception("CommandManager.__checkArgsType, error: arg[" + str(key) + "] " +
                                "value must be a string, not a " + str(type(value)) )

    def doCommand(self, cmd):
        """ Implementation of CommandManager.
            Raises an exception if an error occurs. 
        """
        error = super(CommandManager, self).doCommand_Python(cmd)
        if error:
            raise Exception(error)

    def undoCommand(self):
        """ Implementation of CommandManager.
            Raises an exception if an error occurs. 
        """
        error = super(CommandManager, self).undoCommand_Python()
        if error:
            raise Exception(error)

    def redoCommand(self):
        """ Implementation of CommandManager.
            Raises an exception if an error occurs. 
        """
        error = super(CommandManager, self).redoCommand_Python()
        if error:
            raise Exception(error)

    def clear(self):
        """ Implementation of CommandManager
        """
        super(CommandManager, self).clear()
        self.__flatCommandsStack = []

    def checkCommandArgs(self, cmd, args):
        """ \internal
            Implementation of CommandManager.
            Raises an exception if an error occurs. 
        """
        error = super(CommandManager, self).checkCommandArgs_Python(cmd, args)
        if error:
            raise Exception(error)

    def clearRedoStack(self):
        """ \internal 
            Implementation of CommandManager
        """
        count = self.totalCountAtStackIndex()
        super(CommandManager, self).clearRedoStack()
        self.__flatCommandsStack = self.__flatCommandsStack[:count]

    def commandPushed(self, cmd, isLowCmd):
        """ \internal 
            Implementation of CommandManager
        """
        self.__flatCommandsStack.append(cmd)
        super(CommandManager, self).commandPushed(cmd, isLowCmd)

    def onCommandRegistered(self, cmdName, cmdType, implType):
        """ \internal
            Implementation of CommandRegistryCallback
            Callback when a command is registered.
        """

        # Run-time creation of the function to create the command.
        # The function is added to the "Commands" modules 
        exec('\
def ' + cmdName + '(**kwargs):\n\
    try:\n\
        return GetCommandManager().createCommand("' + cmdName + '", kwargs )\n\
    except Exception as e:\n\
        raise Exception(e)\n\
setattr(GetOrCreateModule("Commands") , "' + cmdName + '", '+ cmdName + ')')


# \internal
# Create the "Commands" module right now so it 
# can be imported with the usual syntax later
# --> import Commands
GetOrCreateModule("Commands")

# \internal
# Store the CommandManager singleton
global s_cmdManagerSingleton
s_cmdManagerSingleton = None

def GetCommandManager():
    """ Gets the CommandManager singleton.
        Raises an exception if the manager is not created. 
    """
    if s_cmdManagerSingleton is None:
        raise Exception("CommandManager.GetCommandManager, the manager is null.\n\
            To create it : CreateCommandManager(FabricCore.Client).")
    else:
        return s_cmdManagerSingleton

def CreateCommandManager(client):
    """ Creates the CommandManager singleton.
    """
    global s_cmdManagerSingleton
    if s_cmdManagerSingleton is None:
        # Be sure the command registry is created.
        CreateCommandRegistry(client)
        s_cmdManagerSingleton = CommandManager(client)
