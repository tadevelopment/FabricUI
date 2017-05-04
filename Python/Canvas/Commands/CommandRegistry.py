#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands

class CommandRegistry(Commands.KLCommandRegistry_Python):

    """ CommandRegistry registers commands used in the Canvas application. It specializes the C++ 
        KLCommandRegistry_Python (./Commands/KLCommandRegistry_Python.h(cpp)) so it can create commands 
        registered in KL, C++ or Python. However, it can register Python command only. The registry is 
        shared between the C++ and Python, so commands defined in Python can be created from C++ code, 
        and vice versa.

        It is a singleton and should not be created directly.
        - Create the registry: cmdRegistry = CommandRegistry(fabricClient)

        - Get the registry: cmdRegistry = GetCommandRegistry()

        Usage:
        - Register a command (Python): cmdRegistry.registerCommand(cmdName, cmdType, userData)

        - Get a command specs [type, implementationType] (Python/C++/KL): cmdRegistry.getCommandSpecs(cmdName)

        - Check a command is registered (Python/C++/KL): cmdRegistry.isCommandRegistered(cmdName)

        - Create a command (Python/C++/KL): cmd = cmdRegistry.createCommand(cmdName)
        
        - Synchronize with the KL registry: cmdRegistry.synchronizeKL()

        In the RegisterCommand method, the userData argument is used to pass optional custom data to the 
        command (C++ void *). The data is referenced by the registry, and given to the command throught 
        the BaseCommand::registrationCallback callback.
    """

    def __init__(self, client): 
        """ Initializes the CommandRegistry.

            Arguments:
            - client: A FabricCore client.
        """
        
        super(CommandRegistry, self).__init__(client)
        # Command implementation types.
        self.COMMAND_PYTHON = 'PYTHON'

        self.__client = client
        # Dictionaty of Command Object types (to construct them)
        # {cmdName, pythonCmdType}
        self.__cmdObjectTypeDict = {}
        # Dictionaty of user data
        self.__cmdUserDataDict = {}

    def registerCommand(self, cmdName, cmdType, userData = None):
        """ Implementation of Commands.CommandRegistry
            Registers a Python command 'cmdType' under the name "cmdName".
            Raises an exception if:
            - The command name is empty, 
            - cmdType is not command (inherite from Commands.BaseCommand)
            - The command has already been registered under another type
        """

        if not cmdName:
            raise Exception('CommandRegistry.registerCommand, error: cmdName is empty') 

        if not issubclass(cmdType, Commands.BaseCommand):
            raise Exception(
                "CommandRegistry.registerCommand, error: command '" + 
                str(cmdName) + "': type '" + 
                str(cmdType) + "' is not a command")

        if super(CommandRegistry, self).isCommandRegistered(cmdName) is False:
            self.__cmdUserDataDict[cmdName] = userData
            self.__cmdObjectTypeDict[cmdName] = cmdType
            self.commandRegistered(cmdName, str(cmdType), self.COMMAND_PYTHON)

        elif cmdType != self.__cmdObjectTypeDict[cmdName]:
            raise Exception(
                "CommandRegistry.registerCommand, error: command '" + str(cmdName) + 
                "': type '" + str(cmdType) + 
                "' overriding previous type '" + str(self.__cmdObjectTypeDict[cmdName]) + "'")
    
    def _createCommand_Python(self, cmdName):
        """ \internal
            Implementation of Commands.CommandRegistry
            Creates and returns a registered command (Python, C++, KL) named "cmdName".
            Raises an exception if the command cannot be created (has to be registered first) 
        """

        if super(CommandRegistry, self).isCommandRegistered(cmdName) is True:
         
            if super(CommandRegistry, self).getCommandSpecs(cmdName)[1] == self.COMMAND_PYTHON:
                # Create the command
                cmd = self.__cmdObjectTypeDict[cmdName]() 
                # Callback -> set name and user-data
                cmd.registrationCallback(
                    cmdName, 
                    self.__cmdUserDataDict[cmdName])

            # Otherwise (C++/KL), create it in C++.
            else:
                error = ""
                cmd = super(CommandRegistry, self)._createCommand_Python(cmdName)

            return cmd

        else:
            raise Exception( 'CommandRegistry.dfscreateCommand, error: ' +
                'cannot create command '' + cmdName + '', it is not registered') 

    def getClient(self):
        """ Implementation of Commands.CommandRegistry
        """
        return self.__client

# \internal
# Store the CommmandRegistry singleton 
global s_cmdRegistrySingleton
s_cmdRegistrySingleton = None

def GetCommandRegistry():
    """ Gets the CommandRegistry singleton.
        Raises an exception if the registry is not created. 
    """
    if s_cmdRegistrySingleton is None:
        raise Exception('CommandRegistry.GetCommandRegistry, the registry is null.\n\
            To create it : CreateCommandRegistry(FabricCore.Client).')
    else:
        return s_cmdRegistrySingleton

def CreateCommandRegistry(client):
    """ Creates the CommandRegistry singleton.
    """
    global s_cmdRegistrySingleton
    if s_cmdRegistrySingleton is None:
        s_cmdRegistrySingleton = CommandRegistry(client)
    return s_cmdRegistrySingleton;
