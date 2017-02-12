#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

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
        if CommandRegistery.registeredCmdMap.get(cmdName) is not None:
            return True
        else:
            return False

    @staticmethod
    def RegisterCommand(cmdName, cmdType, userData = None):
        """ Registers the command cmdType under the name "cmdName".
        """
        if CommandRegistery.IsCommandRegistered(cmdName) is False:
            CommandRegistery.registeredCmdMap[cmdName] = cmdType
            CommandRegistery.registeredCmdUserDataList[cmdName] = userData

    @staticmethod
    def CreateCommand(cmdName):
        """ \Internal, used by the CommandManager.
            Creates a registered command named "cmdName".
            Raises an exception if the command cannot be created (has to be registered first) 
        """
        if CommandRegistery.IsCommandRegistered(cmdName) is True:
            cmd = CommandRegistery.registeredCmdMap[cmdName]()

            userData = CommandRegistery.registeredCmdUserDataList[cmdName]
            cmd.registrationCallBack(cmdName, userData)

            return cmd

        else:
            raise Exception( "Cannot create command, '" + cmdName + "' is not registered") 
 