#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands as CppCommands

class BaseScriptableCommand(CppCommands.BaseScriptableCommand_Python):
    
    """ BaseScriptableCommand specializes BaseScriptableCommand_Python (C++). 
        See Commands/BaseScriptableCommand.h(cpp), and Commands/BaseScriptableCommand_Python.h(cpp). 
    """
    
    def __init__(self):
        """ Initializes the BaseScriptableCommand.
        """
        super(BaseScriptableCommand, self).__init__()

    def declareArg(self, key, flag=2, defaultValue=""):
        """ Impl. of CppCommands.ScriptableCommand
            Declares an argument, called from constructor.
        """
        self._declareArg_Python(key, flag, defaultValue)
    
    def hasArg(self, key):
        """ Impl. of CppCommands.ScriptableCommand
            Checks if the command has an arg.
        """ 
        return super(BaseScriptableCommand, self).hasArg(key)
 
    def isArg(self, key):
        """ Impl. of CppCommands.ScriptableCommand
            Checks if an arg is optional.
        """
        error, hasFlag = self._isArg_Python(key)
        return hasFlag;

    def isArgLoggable(self, key):
        """ Impl. of CppCommands.ScriptableCommand
            Checks if an arg is loggable.
        """
        error, isLoggable = self._isArgLoggable_Python(key)
        return isLoggable;

    def getArgKeys(self):
        """ Impl. of CppCommands.ScriptableCommand
            Gets the arguments keys.
        """
        return super(BaseScriptableCommand, self).getArgKeys()
    
    def isArgSet(self, key):
        """ Impl. of CppCommands.ScriptableCommand
            Checks if an arg has been set, to get safely optional arg.
        """
        return super(BaseScriptableCommand, self).isArgSet(key)

    def getArg(self, key):
        """ Impl. of CppCommands.ScriptableCommand
            Gets an argument.
        """
        error, arg = self._getArg_Python(key) 
        return arg;

    def setArg(self, key, arg):
        """ Impl. of CppCommands.ScriptableCommand
            Sets an argument, called from the manager.
        """
        self._setArg_Python(key, arg)

    def validateSetArgs(self):
        """ Impl. of CppCommands.ScriptableCommand
            Checks the args are correctly set before doing the command, 
            called from the manager.
        """
        self._validateSetArgs_Python()

    def getArgsDescription(self):
        """ Impl. of CppCommands.ScriptableCommand
            Gets a decription of the args, for debugging.
        """
        return super(BaseScriptableCommand, self).getArgsDescription()

    ### \internal, don't call these methods.
    def _declareArg_Python(self, key, flags, defaultValue):
        """ \internal, impl. of CppCommands.BaseScriptableCommand_Python. """
        error = super(BaseScriptableCommand, self)._declareArg_Python(
            key, flags, defaultValue)
        if error:
            raise Exception(error)  
        return error;

    def _isArg_Python(self, key, flag):
        """ \internal, impl. of CppCommands.BaseScriptableCommand_Python. """
        error, hasFlag = super(BaseScriptableCommand, self)._isArg_Python(
            key, flag)
        if error:
            raise Exception(error)  
        return error, hasFlag;

    def _getArg_Python(self, key):
        """ \internal, impl. of CppCommands.BaseScriptableCommand_Python. """
        error, arg = super(BaseScriptableCommand, self)._getArg_Python(key)
        if error:
            raise Exception(error)  
        return error, arg;

    def _setArg_Python(self, key, arg):
        """ \internal, impl. of CppCommands.BaseScriptableCommand_Python. """
        error = super(BaseScriptableCommand, self)._setArg_Python(
            key, arg)
        if error:
            raise Exception(error)  
        return error;

    def _validateSetArgs_Python(self):
        """ \internal, impl. of CppCommands.BaseScriptableCommand_Python. """
        error = super(BaseScriptableCommand, self)._validateSetArgs_Python()
        if error:
            raise Exception(error)  
        return error;