#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands

class CommandRegistery():

    """ CommandRegistery registers commands used in the Canvas application.
       
        To register a command: CommandRegistery.RegisterCommand(cmdName, cmdType, userData)
        To create a command: cmd = CommandRegistery.CreateCommand(cmdName)
        To check that a command is registered: CommandRegistery.IsCommandRegistered(cmdName)

        The userData argument is used to pass optional custom data to the command (C++ void *).
        The data is referenced by the registery, and given to the command throught 
        the BaseCommand::registrationCallBack callback.
    """

    # \internal
    # Dictionaries of registered commands {cmdName, cmdType} and user Data {cmdName, userData}
    registeredCmdMap = {}
    registeredCmdUserDataList = {}
 
    @staticmethod
    def IsCommandRegistered(cmdName):
        """ Checks if a command has been registered under the name "cmdName".
        """           
        cmdType = CommandRegistery.registeredCmdMap.get(cmdName)
        if cmdType is not None:
            return [True, cmdType]
        else:
            return [False, None]

    @staticmethod
    def RegisterCommand(cmdName, cmdType, userData = None):
        """ Registers the command cmdType under the name "cmdName".
            Raises an exception if the command name is empty, 
            or cmdType is not command (inherite from Commands.BaseCommand),
            or if the command has already been registered under another type.
        """
        if not cmdName:
            raise Exception("Error CommandRegistery.RegisterCommand: cmdName is empty") 

        if not issubclass(cmdType, Commands.BaseCommand):
            raise Exception("Error CommandRegistery.RegisterCommand: command '" + str(cmdName) + "': type '" + str(cmdType) + "' is not a Command")

        [isRegistered, existingCmdType] = CommandRegistery.IsCommandRegistered(cmdName)
        if isRegistered is False:
            CommandRegistery.registeredCmdMap[cmdName] = cmdType
            CommandRegistery.registeredCmdUserDataList[cmdName] = userData

        elif cmdType != existingCmdType:
            error = "Error CommandRegistery.RegisterCommand: command '" + str(cmdName) + "': type '" + str(cmdType) + "' overriding previous type '" + str(existingCmdType) + "'"
            raise Exception(error)
   
    @staticmethod
    def CreateCommand(cmdName):
        """ \Internal, used by the CommandManager.
            Creates a registered command named "cmdName".
            Raises an exception if the command cannot be created (has to be registered first) 
        """
        if CommandRegistery.IsCommandRegistered(cmdName)[0] is True:
            cmd = CommandRegistery.registeredCmdMap[cmdName]()

            userData = CommandRegistery.registeredCmdUserDataList[cmdName]
            cmd.registrationCallBack(cmdName, userData)

            return cmd

        else:
            raise Exception( "Cannot create command, '" + cmdName + "' is not registered") 
 