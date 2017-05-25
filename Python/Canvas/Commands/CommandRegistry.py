#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands as CppCommands

class CommandRegistry(CppCommands.KLCommandRegistry_Python):

    """ CommandRegistry registers commands used in the Canvas application. It specializes the C++ 
        KLCommandRegistry_Python (./Commands/KLCommandRegistry_Python.h(cpp)) so it can create commands 
        registered in KL, C++ or Python. However, it can register Python command only. The registry is 
        shared between the C++ and Python, so commands defined in Python can be created from C++ code, 
        and vice versa.

        It is a singleton and should not be created directly.
        - Create the registry: cmdRegistry = CommandRegistry(fabricClient)

        - Get the registry: cmdRegistry = GetCmdRegistry()

        Available methods:
        - Register a command (Python): cmdRegistry.registerCmd(cmdName, cmdType, userData)

        - Get a command specs [type, implementationType] (Python/C++/KL): cmdRegistry.getCmdSpecs(cmdName)

        - Check a command is registered (Python/C++/KL): cmdRegistry.isCmdRegistered(cmdName)

        - Create a command (Python/C++/KL): cmd = cmdRegistry.createCmd(cmdName)
        
        - Get the stack content as string: cmdRegistry.getContent()

        - Synchronize with the KL registry: cmdRegistry.synchronizeKL()

        In the RegisterCommand method, the userData argument is used to pass optional custom data to the 
        command (C++ void *). The data is referenced by the registry, and given to the command throught 
        the BaseCommand::registrationCallback callback.
    """

    def __init__(self): 
        """ Initializes the CommandRegistry.
        """
        super(CommandRegistry, self).__init__()
        # Command implementation types.
        self.COMMAND_PYTHON = 'PYTHON'

        # Dictionaty of Command Object types (to construct them)
        # {cmdName, pythonCmdType}
        self.__cmdObjectTypeDict = {}
        # Dictionaty of user data
        self.__cmdUserDataDict = {}

    def registerCmd(self, cmdName, cmdType, userData = None):
        """ Registers a Python command 'cmdType' under the name "cmdName".
            Raises an exception if:
            - The command name is empty, 
            - cmdType is not command (inherite from Commands.BaseCommand)
            - The command has already been registered under another type
        """
        if not cmdName:
            raise Exception('CommandRegistry.registerCommand, error: cmdName is empty') 

        if not issubclass(cmdType, CppCommands.BaseCommand):
            raise Exception(
                "CommandRegistry.registerCommand, error: command '" + 
                str(cmdName) + "': type '" + 
                str(cmdType) + "' is not a command")

        if super(CommandRegistry, self).isCommandRegistered(cmdName) is False:
            self.__cmdUserDataDict[cmdName] = userData
            self.__cmdObjectTypeDict[cmdName] = cmdType
            self._commandIsRegistered_Python(cmdName, str(cmdType), self.COMMAND_PYTHON)

        elif cmdType != self.__cmdObjectTypeDict[cmdName]:
            raise Exception(
                "CommandRegistry.registerCommand, error: command '" + str(cmdName) + 
                "': type '" + str(cmdType) + 
                "' overriding previous type '" + str(self.__cmdObjectTypeDict[cmdName]) + "'")
    
    def createCmd(self, cmdName):
        """ Creates and returns a registered command (C++/Python/KL) named "cmdName".
            Raises an exception if the command cannot be created (has to be registered first) 
        """
        error, cmd = self._createCommand_Python(cmdName)
        return cmd

    def isCmdRegistered(self, cmdName):
        """ Checks if a command (C++/Python/KL) 
            has been registered under 'cmdName'.
        """
        return self.isCommandRegistered(cmdName)

    def getCmdSpecs(self, cmdName):
        """ Gets the command and implementation type (C++/Python/KL).
            Returns an empty list if the command is not registred.
        """
        return self.getCommandSpecs(cmdName)

    def synchronizeKL(self):
        self._synchronizeKL_Python()

    ### \internal
    def _createCommand_Python(self, cmdName):
        """ \internal Implementation of Commands.CommandRegistry
        """
        if super(CommandRegistry, self).isCommandRegistered(cmdName) is True:
         
            if super(CommandRegistry, self).getCommandSpecs(cmdName)[1] == self.COMMAND_PYTHON:
            
                try:
                    # Create the command
                    cmd = self.__cmdObjectTypeDict[cmdName]() 
                    # Callback -> set name and user-data
                    cmd.registrationCallback(
                        cmdName, 
                        self.__cmdUserDataDict[cmdName])
                except Exception as e:    
                    raise Exception(e)

                return ['', cmd]

            # Otherwise (C++/KL), create it in C++.
            else:
                error, cmd = super(CommandRegistry, self)._createCommand_Python(cmdName)
                if len(error) > 0:
                    raise Exception(error)  
                return [error, cmd]

        else:
            raise Exception("CommandRegistry._createCommand_Python, error: " +
                "cannot create command '" + cmdName + "', it is not registered") 

    def _commandIsRegistered_Python(self, cmdName, cmdType, implType):
        """ \internal, impl. of Commands.KLCommandRegistry_Python. """
        error = super(CommandRegistry, self)._commandIsRegistered_Python(
            cmdName, cmdType, implType)  
        if error:
            raise Exception(error)  
        return error;

    def _synchronizeKL_Python(self):
        """ \internal, impl. of Commands.KLCommandRegistry_Python. """
        error = super(CommandRegistry, self)._synchronizeKL_Python()
        if error:
            raise Exception(error)
        return error;

# \internal, store the registry singleton 
global s_cmdRegistrySingleton
s_cmdRegistrySingleton = None

def GetCmdRegistry():
    return CreateCmdRegistry()

def CreateCmdRegistry():
    """ Creates the CommandRegistry singleton.
    """
    global s_cmdRegistrySingleton
    if s_cmdRegistrySingleton is None:
        s_cmdRegistrySingleton = CommandRegistry()
    return s_cmdRegistrySingleton
