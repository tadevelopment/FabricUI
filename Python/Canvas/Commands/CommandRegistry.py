#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands
 
class CommandRegistry(Commands.CommandRegistry):

    """ CommandRegistry registers commands used in the Canvas application. It specializes the C++ 
        CommandRegistry (./Commands/CommandRegistry.h(cpp)) so it can create commands registered in, 
        KL, C++ or Python. However, it can register Python command only. The registry is shared between 
        the C++ and Python, so commands defined in Python can be created from C++ code, and vice versa.

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
        # Dictionaty of Command Object types (to construct them)
        # {cmdName, pythonCmdType}
        self.cmdObjectTypeDict = {}
        # Dictionaty of user data
        self.cmdUserDataDict = {}

    def registerCommand(self, cmdName, cmdType, userData = None):
        """ Registers a Python command 'cmdType' under the name "cmdName".
            Raises an exception if:
            - The command name is empty, 
            - cmdType is not command (inherite from Commands.BaseCommand)
            - The command has already been registered under another type
        """

        if not cmdName:
            raise Exception("CommandRegistry.RegisterCommand, error: cmdName is empty") 

        if not issubclass(cmdType, Commands.BaseCommand):
            raise Exception(
                "CommandRegistry.RegisterCommand, error: command '" + 
                str(cmdName) + "': type '" + 
                str(cmdType) + "' is not a command")

        if super(CommandRegistry, self).isCommandRegistered(cmdName) is False:
            self.cmdUserDataDict[cmdName] = userData
            self.cmdObjectTypeDict[cmdName] = cmdType
            self.commandRegistered(cmdName, str(cmdType), Commands.CommandRegistry.COMMAND_PYTHON)

        elif cmdType != self.cmdObjectTypeDict[cmdName]:
            raise Exception(
                "CommandRegistry.RegisterCommand, error: command '" + str(cmdName) + 
                "': type '" + str(cmdType) + 
                "' overriding previous type '" + str(self.cmdObjectTypeDict[cmdName]) + "'")
   
    def createCommand(self, cmdName):
        """ Creates and returns a registered command (Python, C++, KL) named "cmdName".
            Raises an exception if the command cannot be created (has to be registered first) 
        """
        
        if super(CommandRegistry, self).isCommandRegistered(cmdName) is True:

            # If the command's is registered in Python, create it here
            if super(CommandRegistry, self).getCommandSpecs(cmdName)[1] == Commands.CommandRegistry.COMMAND_PYTHON:
                # Create the command
                cmd = self.cmdObjectTypeDict[cmdName]() 
                # Callback -> set name and user-data
                cmd.registrationCallback(
                    cmdName, 
                    self.cmdUserDataDict[cmdName])

            # Otherwise (C++/KL), create it in C++.
            else:
                cmd = super(CommandRegistry, self).createCommand(cmdName)

            return cmd

        else:
            raise Exception( "CommandRegistry.createCommand, error: " +
                "cannot create command '" + cmdName + "', it is not registered") 

# \internal
# Store the CommmandRegistry singleton 
global s_cmdRegistrySingleton
s_cmdRegistrySingleton = None

def GetCommandRegistry():
    """ Gets the CommandRegistry singleton.
        Raises an exception if the registry is not created. 
    """
    if s_cmdRegistrySingleton is None:
        raise Exception("CommandRegistry.GetCommandRegistry, the registry is null.\n\
            To create it : CreateCommandRegistry(FabricCore.Client).")
    else:
        return s_cmdRegistrySingleton

def CreateCommandRegistry(client):
    """ Creates the CommandRegistry singleton.
    """
    global s_cmdRegistrySingleton
    if s_cmdRegistrySingleton is None:
        s_cmdRegistrySingleton = CommandRegistry(client)
