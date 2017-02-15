#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands

class CommandRegistry(Commands.CommandRegistry):

    """ CommandRegistry registers commands used in the Canvas application.
       
        To register a command: CommandRegistry.RegisterCommand(cmdName, cmdType, userData)
        To create a command: cmd = CommandRegistry.CreateCommand(cmdName)
        To check that a command is registered: CommandRegistry.IsCommandRegistered(cmdName)

        The userData argument is used to pass optional custom data to the command (C++ void *).
        The data is referenced by the registery, and given to the command throught 
        the BaseCommand::registrationCallBack callback.
    """

    # \internal
    # Dictionaries of registered commands {cmdName, cmdType} and user Data {cmdName, userData}
    registeredCmds = {}
    registeredCmdUserDataList = {}
 
    @staticmethod
    def IsCommandRegistered(cmdName):
        """ Checks if a command has been registered under the name "cmdName".
        """           
        cmdType = CommandRegistry.registeredCmds.get(cmdName)
        if cmdType is not None:
            return [True, cmdType, "Python"]
        else:
            if Commands.CommandRegistry.IsCommandRegistered(cmdName, cmdType) is True:
                return [True, cmdType, "C++"]
            else:
                return [False, None, None]

    @staticmethod
    def RegisterCommand(cmdName, cmdType, userData = None):
        """ Registers the command cmdType under the name "cmdName".
            Raises an exception if the command name is empty, 
            or cmdType is not command (inherite from Commands.BaseCommand),
            or if the command has already been registered under another type.
        """
        if not cmdName:
            raise Exception("Error CommandRegistry.RegisterCommand: cmdName is empty") 

        if not issubclass(cmdType, Commands.BaseCommand):
            raise Exception("Error CommandRegistry.RegisterCommand: command '" + str(cmdName) + "': type '" + str(cmdType) + "' is not a Command")

        [isRegistered, existingCmdType, From] = CommandRegistry.IsCommandRegistered(cmdName)
        if isRegistered is False:
            CommandRegistry.registeredCmds[cmdName] = cmdType
            CommandRegistry.registeredCmdUserDataList[cmdName] = userData
            Commands.CommandRegistry.s_cmdRegistry.commandIsRegistered(cmdName)

        elif cmdType != existingCmdType:
            error = "Error CommandRegistry.RegisterCommand: command '" + str(cmdName) + "': type '" + str(cmdType) + "' overriding previous type '" + str(existingCmdType) + "'"
            raise Exception(error)
   
    @staticmethod
    def CreateCommand(cmdName):
        """ \Internal, used by the CommandManager.
            Creates a registered command named "cmdName".
            Raises an exception if the command cannot be created (has to be registered first) 
        """

        cmd = None
        [isRegistred, cmdType, From] = CommandRegistry.IsCommandRegistered(cmdName)
        
        if isRegistred is True:
            if From == "Python":
                cmd = CommandRegistry.registeredCmds[cmdName]()
            elif From == "C++":
                cmd = Commands.CommandRegistry.CreateCommand(cmdName)

        if cmd is not None:
            userData = CommandRegistry.registeredCmdUserDataList[cmdName]
            cmd.registrationCallBack(cmdName, userData)
            return cmd

        else:
            raise Exception( "Cannot create command, '" + cmdName + "' is not registered") 

    def commandIsRegistered(self, cmdName):
        print str(cmdName)
 