#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.Canvas.Utils import *
from FabricEngine.FabricUI import Commands
from FabricEngine.Canvas.Commands.CommandRegistry import *

class CommandManager(Commands.KLCommandManager_Python):
    
    """ CommandManager specializes KLCommandManager_Python (C++). See Commands/KLCommandManager.h(cpp) 
        and Commands/KLCommandManager_Python.h(cpp). The manager is shared between the C++ and Python, 
        so commands defined in Python can be created from C++ code too, and vice versa.

        The CommandManager is a singleton and should not be created directly.
        - Create the singleton: cmdManager = CommandManager(fabricClient)

        - Get the singleton: cmdManager = GetCommandManager()

        Usage:
        - Create a command (KL/C++/Python): cmd = cmdManager.createCmd(cmdName, args, doIt)
                
        - Execute a command: cmdManager.doCommand(cmd);

        - Proces undo: cmdManager.undoCommand()

        - Proces redo: cmdManager.redoCommand()

        - Synchronize with the KL manager: cmdManager.synchronizeKL()

        When a command is registered, a function creating the command with named arguments is dynimically  
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
        # There is no "new" is python, so we need to own the commands 
        # created in Python. They are refered by the C++ CommandManager 
        # undo-redo stacks. 
        self.__client = client
        self.__flatCommandsStack = []

        # Connect our-self.
        cmdRegistry = CreateCommandRegistry(client)
        cmdRegistry.commandRegistered.connect(self.__onCommandRegistered)
    
    def createCmd(self, cmdName, args = {}, doCmd = True):
        """ Initializes the CommandManager.

            Arguments:
            - cmdName: A FabricCore client.
            - args: A FabricCore client.
            - doCmd: A FabricCore client.
        """
              
        try:
            cmd = GetCommandRegistry()._createCommand_Python(cmdName)
            
            if len(args) > 0:
                self.__castAndCheckCommandArgs(cmd, args);

            if doCmd :
                self.doCommand(cmd)
          
            return cmd

        except Exception as e:    
            raise Exception(e)

    def doCommand(self, cmd):
        """ Implementation of Commands.CommandManager.
            Raises an exception if an error occurs. 
        """
        error = super(CommandManager, self)._doCommand_Python(cmd)
        if error:
            raise Exception(error)

    def undoCommand(self):
        """ Implementation of Commands.CommandManager.
            Raises an exception if an error occurs. 
        """
        error = super(CommandManager, self)._undoCommand_Python()
        if error:
            raise Exception(error)

    def redoCommand(self):
        """ Implementation of Commands.CommandManager.
            Raises an exception if an error occurs. 
        """
        error = super(CommandManager, self)._redoCommand_Python()
        if error:
            raise Exception(error)

    def clear(self):
        """ Implementation of Commands.CommandManager
        """
        super(CommandManager, self).clear()
        self.__flatCommandsStack = []

    def getClient(self):
        """ Implementation of Commands.CommandManager
        """
        return self.__client

    def __castInputArgsToStr(self, cmd, inputArgs):
        """ \internal
            Casts the inputs arguments of the command into String. 
        """

        try:
            strArgs = {}

            for key, value in inputArgs.iteritems():
                
                inputArgType = type(value)
                isInputArgTypeStr = issubclass(inputArgType, str) or issubclass(inputArgType, unicode)

                # Commands.BaseRTValScriptableCommand
                if issubclass(type(cmd), Commands.BaseRTValScriptableCommand):

                    # The input arg is a RTVal, cast it to JSON.
                    # Construct a dumb RTVal to compare the python
                    # object type --> find a better way.
                    if inputArgType == type(self.__client.RT.types.UInt32()):
                        value = value.getJSONStr()

                    else:
                        # Get the KL RTVal type.
                        rtValType = cmd.getRTValArgType(key)
                        isArgStr = rtValType == "String"

                        # The arg is Python. If the input arg is a string and the
                        # cmd arg is not, assume the input arg is already the JSON. 
                        # Otherwise we create the JSON from the pyton value.
                        if not (isInputArgTypeStr and not isArgStr):
                            castArg = True

                            # Check if the string has quotes.
                            # If so, we assume that it's already a JSON
                            if isInputArgTypeStr and isArgStr and len(value) > 0:
                                first =  value[0] == "'" or value[0] == "\"" 
                                last =  value[len(value)-1] == "'" or value[len(value)-1] == "\"" 
                                castArg = not first and not last

                            if castArg:
                                # Get the python RTVal object.
                                pyRTValType = getattr(self.__client.RT.types, rtValType)

                                # Check if the python type can be casted as a RTVal.
                                if pyRTValType().getSimpleType() is None:
                                    raise Exception("Can't cast python '" + str(inputArgType) + 
                                        "' to rtVal '" + str(rtValType) + "'")
                                
                                # Construct the python RTVal and sets its value.
                                rtVal = pyRTValType(value)   
                                value = rtVal.getJSONStr()   
                
                # Commands.BaseScriptableCommand, all in strings
                else:
                    value = str(value)   

                strArgs[key] = value
                               
        except Exception as e:    
            raise Exception("CommandManager.__castInputArgsToStr, error: " + str(e) )
        
        return strArgs

    def __castInputArgsToTRVal(self, cmd, inputArgs):
        """ \internal
            Casts the inputs arguments of the command into RTVal. 
        """
        try:
            rtValArgs = {}

            for key, value in inputArgs.iteritems():
                if type(value) != type(self.__client.RT.types.UInt32()):
                    rtValType = cmd.getRTValArgType(key)
                    pyRTValType = getattr(self.__client.RT.types, rtValType)
                    value  = pyRTValType(value)  
                rtValArgs[key] = value   
                   
            return rtValArgs
        
        except Exception as e:    
            raise Exception(
                'CommandManager.__castInputArgsToTRVal, error: ' + str(e) )

    def __castAndCheckCommandArgs(self, cmd, inputArgs):
        """ \internal
            Casts the inputs arguments of the command into RTVal. 
        """

        if not issubclass(type(cmd), Commands.BaseScriptableCommand):
            raise Exception(
                "CommandManager.__castAndCheckCommandArgs, error: Command '" +
                str(cmd.getName()) + "' is not scriptable" )

        # Check if it's a BaseRTValScriptableCommand.
        isRTValScriptCommand = issubclass(type(cmd), Commands.BaseRTValScriptableCommand)
        createRTValCommand = isRTValScriptCommand

        for key, value in inputArgs.iteritems():
            if not cmd.hasArg(key):
                raise Exception(
                    "CommandManager.__castAndCheckCommandArgs, error: arg '" + 
                    str(key) + "' doesn't exist" )

            if isRTValScriptCommand:
                rtValType = cmd.getRTValArgType(key)
                if len(rtValType) == 0:
                    createRTValCommand = False
                    break;

        if isRTValScriptCommand and createRTValCommand:
            args = self.__castInputArgsToTRVal(cmd, inputArgs)
            super(CommandManager, self)._checkRTValCommandArgs_Python(cmd, args)

        else:
            args = self.__castInputArgsToStr(cmd, inputArgs)
            super(CommandManager, self)._checkCommandArgs_Python(cmd, args)

    def _createCommand_Python(self, cmdName, args = {}, doCmd = True):
        """ \internal, 
            Implementation of KLCommandManager_Python
        """
        try:
            cmd = GetCommandRegistry()._createCommand_Python(cmdName)
            
            if len(args) > 0:
                super(CommandManager, self)._checkCommandArgs_Python(cmd, args)

            if doCmd :
                self.doCommand(cmd)
                
            return cmd

        except Exception as e:    
            raise Exception(e)

    def _createRTValCommand_Python(self, cmdName, args = {}, doCmd = True):
        """ \internal, 
            Implementation of KLCommandManager_Python
        """
        try:
            cmd = GetCommandRegistry()._createCommand_Python(cmdName)

            if len(args) > 0:
                super(CommandManager, self)._checkRTValCommandArgs_Python(cmd, args)

            if doCmd :
                self.doCommand(cmd)
                
            return cmd
        
        except Exception as e:    
            raise Exception(e)

    def clearRedoStack(self):
        """ \internal 
            Implementation of Commands.CommandManager
        """
        count = self._totalCountAtStackIndex()
        super(CommandManager, self).clearRedoStack()
        self.__flatCommandsStack = self.__flatCommandsStack[:count]

    def _commandIsPushed_Python(self, cmd, isLowCmd):
        """ \internal 
            Implementation of KLCommandManager_Python
        """
        self.__flatCommandsStack.append(cmd)

    def __onCommandRegistered(self, cmdName, cmdType, implType):
        """ \internal
            Callback when a command is registered.
        """

        # Run-time creation of the function to create the command.
        # The function is added to the "Commands" module.
        exec('\
def ' + cmdName + '(**kwargs):\n\
    print "__onCommandRegistered"\n\
    try:\n\
        return GetCommandManager().createCmd("' + cmdName + '", kwargs )\n\
    except Exception as e:\n\
        raise Exception(e)\n\
setattr(GetOrCreateModule("Commands") , "' + cmdName + '", '+ cmdName + ')')


# \internal
# Create the "Commands" module right now so it 
# can be imported with the usual syntax later
# --> import Commands
GetOrCreateModule('Commands')

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
        s_cmdManagerSingleton = CommandManager(client)
    return s_cmdManagerSingleton;
