#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands
from FabricEngine.Canvas.GlobalInjector import GlobalInjector

class CommandRegistry(Commands.CommandRegistry):

    """ CommandRegistry registers commands used in the Canvas application.
        It specializes the C++ CommandRegistry (Commands/CommandRegistry.h(cpp))
        so it can create commands registered in C++ or Python. However, it can 
        register Python command only.

        It is a singleton and should not be created directly.
        - Get the registry: cmdRegistry = GetCommandRegistry()

        The registery is shared between the C++ and Python, so commands defined in Python 
        can be called from C++ code too, and vice versa.

        - Register a command (Python only): cmdRegistry.registerCommand(cmdName, cmdType, userData)

        - Check a command is registered (Python/C++): cmdRegistry.isCommandRegistered(cmdName)

        - Create a command (Python/C++): cmd = cmdRegistry.createCommand(cmdName)

        In the RegisterCommand method, the userData argument is used to pass optional 
        custom data to the command (C++ void *). The data is referenced by the registery, 
        and given to the command throught the BaseCommand::registrationCallBack callback.
    """

    def __init__(self): 
        super(CommandRegistry, self).__init__()
        
        # !!! Very important !!!!!
        # Here, we set the C++ pointer of the C++ CommandRegistry 
        # singleton equal to this python instance of CommandRegistry.
        Commands.CommandRegistry.SetCommandRegistrySingleton(self)
        # Dictionaries of registered commands (Python/C++): 
        # {cmdName, {From, type, userData}}
        self.__registeredCmds = {}

    def isCommandRegistered(self, cmdName):
        """ Checks if a command (Python or C++) has been registered under "cmdName", 

            Returns a list [isRegistered, cmdType, From]:
            - isRegistered: If true, the command is registered
            - cmdType: Type of the commands
            - From: Tell 'Python' or 'C++'
        """       

        cmdInfo = self.__registeredCmds.get(cmdName)

        if cmdInfo is not None and cmdInfo['type'] is not None:
            return [True, cmdInfo['type'], cmdInfo['From']]

        return [False, None, None]

    def registerCommand(self, cmdName, cmdType, userData = None):
        """ Registers a Python command 'cmdType' under the name "cmdName".
            Raises an exception if:
            - The command name is empty, 
            - cmdType is not command (inherite from Commands.BaseCommand)
            - The command has already been registered under another type
        """

        if not cmdName:
            raise Exception("Error CommandRegistry.RegisterCommand: cmdName is empty") 

        if not issubclass(cmdType, Commands.BaseCommand):
            raise Exception("Error CommandRegistry.RegisterCommand: command '" + str(cmdName) + "': type '" + str(cmdType) + "' is not a Command")

        [isRegistered, existingCmdType, From] = self.isCommandRegistered(cmdName)
        
        if isRegistered is False:
            self.__commandIsRegistered("Python", cmdName, cmdType, userData)

        elif cmdType != existingCmdType:
            error = "Error CommandRegistry.RegisterCommand: command '" + str(cmdName) + "': type '" + str(cmdType) + "' overriding previous type '" + str(existingCmdType) + "'"
            raise Exception(error)
   
    def createCommand(self, cmdName):
        """ Creates and returns a registered command (Python or C++) named "cmdName".
            Raises an exception if the command cannot be created (has to be registered first) 
        """

        [isRegistred, cmdType, From] = self.isCommandRegistered(cmdName)
        
        if isRegistred is True:

            cmd = None
            if From == "Python":
                # Create the command
                cmd = cmdType()
                # Callback -> set name and user-data
                cmd.registrationCallBack(
                    cmdName, 
                    self.__registeredCmds[cmdName]['userData'])

            elif From == "C++":
                # If the command's been registered in C++, create it in C++.
                cmd = super(CommandRegistry, self).createCommand(cmdName)

            return cmd

        else:
            raise Exception( "Cannot create command '" + cmdName + "', it is not registered") 

    def commandIsRegistered(self, cmdName, cmdType):
        """ \Internal 
            Implementation of CommandRegistry (C++),
            Callback when a command is registered in C++.
        """

        self.__commandIsRegistered("C++", cmdName, cmdType, None)

    def __commandIsRegistered(self, From, cmdName, cmdType, userData):
        """ \Internal
            Callback when a command is registered in C++ pr Python.
        """

        # Register the command info
        self.__registeredCmds[cmdName] = {
            'From':From, 
            'type':cmdType, 
            'userData':userData
        }

        # Register the command info
        exec('\
def ' + cmdName + '(**kwargs):\n\
    from FabricEngine.Canvas.CommandManager import *\n\
    try:\n\
        return GetCommandManager().createCommand("' + cmdName + '", kwargs )\n\
    except Exception as e:\n\
        raise Exception(e)\n\
setattr(GlobalInjector(), "' + cmdName + '", '+ cmdName + ')')


# \internal
# !!!!! Store the reference to the 
# CommmandRegistery singleton !!!!
s_registery = CommandRegistry()

def GetCommandRegistry():
    """ Gets the CommandRegistry singleton.
    """
    return Commands.CommandRegistry.GetCommandRegistry()