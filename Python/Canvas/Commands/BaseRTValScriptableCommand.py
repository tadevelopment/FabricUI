#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands as CppCommands

class BaseRTValScriptableCommand(CppCommands.BaseRTValScriptableCommand_Python):
    
    """ BaseRTValScriptableCommand specializes BaseRTValScriptableCommand_Python (C++). 
        See Commands/BaseRTValScriptableCommand, Commands/BaseRTValScriptableCommand_Python. 

        Because RTVal can be represented as JSON (string), JSON can be used to set an argument. 
        We use this feature if the type of the RTVal is not known when the argument is declared 
        and the command is executed from the script-editor. 
    """
    
    def __init__(self):
        """ Initializes the BaseRTValScriptableCommand.
        """
        super(BaseRTValScriptableCommand, self).__init__()

    # ScriptableCommand
    def declareArg(self, key, flag=2, defaultValue=""):
        """ Impl. of CppCommands.ScriptableCommand
            Declares an arg. of unknown RTVal type.
            The parameter 'defaultValue' does nothing.
            To set the argument as JSON, use `setArg`.
        """
        self._declareArg_Python(key, flag, defaultValue)
    
    def hasArg(self, key):
        """ Impl. of CppCommands.ScriptableCommand
            Checks if the command has an arg.
        """ 
        return super(BaseRTValScriptableCommand, self).hasArg(key)
 
    def isArg(self, key, flag):
        """ Impl. of CppCommands.ScriptableCommand
            Checks if an arg is flag.
        """
        error, hasFlag = self._isArg_Python(key, flag)
        return hasFlag;

    def getArgKeys(self):
        """ Impl. of CppCommands.ScriptableCommand
            Gets the arguments keys.
        """
        return super(BaseRTValScriptableCommand, self).getArgKeys()
    
    def isArgSet(self, key):
        """ Impl. of CppCommands.ScriptableCommand
            Checks if an arg has been set, to get safely flag arg.
        """
        return super(BaseRTValScriptableCommand, self).isArgSet(key)

    def getArg(self, key):
        """ Impl. of CppCommands.ScriptableCommand
            Gets the JSON representation of the arg.
        """
        error, arg = self._getArg_Python(key) 
        return arg;

    def setArg(self, key, arg):
        """ Impl. of CppCommands.ScriptableCommand
            Sets a JSON representation of the arg. To use when the RTVal type 
            of the arg is unknown (e.g set from the script-editor). Throws an 
            error if the key is empty or hasn't been declared.
        """
        self._setArg_Python(key, arg)

    def validateSetArgs(self):
        """ Impl. of CppCommands.ScriptableCommand
            Checks the args are correctly set (as RTVal or as JSON) before doing 
            the command, called from the manager.
        """
        self._validateSetArgs_Python()

    def getArgsDescription(self):
        """ Impl. of CppCommands.ScriptableCommand
            Gets a decription of the args, for debugging.
        """
        return super(BaseRTValScriptableCommand, self).getArgsDescription()
    
    # RTValScriptableCommand
    def declareRTValArg(self, key, type_, flag=2, defaultValue=None):
        """ Impl. of CppCommands.RTValScriptableCommand
            Declares an argument, called from constructor.
        """
        self._declareRTValArg_Python(
            key, type_, flag, defaultValue)
         
    def getRTValArgType(self, key):
        """ Impl. of CppCommands.RTValScriptableCommand
            Gets the argument rtval type.
        """ 
        error, type_ = self._getRTValArgType_Python(key) 
        return type_;

    def setRTValArgType(self, key, type_):
        """ Impl. of CppCommands.RTValScriptableCommand
            Sets the argument type if not set.
        """ 
        self._setRTValArgType_Python( key, type_)

    def getRTValArg(self, key):
        """ Impl. of CppCommands.RTValScriptableCommand
            Gets an argument.
        """ 
        error, arg = self._getRTValArg_Python(key)  
        return arg;

    def getRTValArg(self, key, type_):
        """ Impl. of CppCommands.RTValScriptableCommand
            To use with unknown type argument. The RTVal 
            is constructed from the JSON and the type
        """ 
        error, arg = self._getRTValArg_Python(key, type_)
        return arg;

    def setRTValArg(self, key, arg):
        """ Impl. of CppCommands.RTValScriptableCommand
            Sets the argument type if not set.
        """ 
        self._setRTValArg_Python(key, arg)
  
 
    ### \internal, don't call these methods.
    # ScriptableCommand
    def _declareArg_Python(self, key, flag, defaultValue):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error = super(BaseRTValScriptableCommand, self)._declareArg_Python(
            key, flag, defaultValue)
        if error:
            raise Exception(error)  
        return error;

    def _isArg_Python(self, key, flag):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error, hasFlag = super(BaseRTValScriptableCommand, self)._isArg_Python(
            key, flag)
        if error:
            raise Exception(error)  
        return error, hasFlag;

    def _getArg_Python(self, key):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error, arg = super(BaseRTValScriptableCommand, self)._getArg_Python(key)
        if error:
            raise Exception(error)  
        return error, arg;

    def _setArg_Python(self, key, arg):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error = super(BaseRTValScriptableCommand, self)._setArg_Python(
            key, arg)
        if error:
            raise Exception(error)  
        return error;

    def _validateSetArgs_Python(self):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error = super(BaseRTValScriptableCommand, self)._validateSetArgs_Python()
        if error:
            raise Exception(error)  
        return error;

    # RTValScriptableCommand
    def _declareRTValArg_Python(self, key, type_, flag, defaultValue):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error = super(BaseRTValScriptableCommand, self)._declareRTValArg_Python(
            key, type_, flag, defaultValue)
        if error:
            raise Exception(error)  
        return error;

    def _getRTValArgType_Python(self, key):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error, type_ = super(BaseRTValScriptableCommand, self)._getRTValArgType_Python(key)
        if error:
            raise Exception(error)  
        return error, type_;

    def _setRTValArgType_Python(self, key, type_):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error = super(BaseRTValScriptableCommand, self)._setRTValArgType_Python(
            key, type_)
        if error:
            raise Exception(error)  
        return error;

    def _getRTValArg_Python(self, key):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error, arg = super(BaseRTValScriptableCommand, self)._getRTValArg_Python(key)
        if error:
            raise Exception(error)  
        return error, arg;

    def _getRTValArg_Python(self, key, type_):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error, arg = super(BaseRTValScriptableCommand, self)._getRTValArg_Python(
            key, type_)
        if error:
            raise Exception(error)  
        return error, arg;

    def _setRTValArg_Python(self, key, arg):
        """ \internal, impl. of CppCommands.BaseRTValScriptableCommand_Python. """
        error = super(BaseRTValScriptableCommand, self)._setRTValArg_Python(
            key, arg)
        if error:
            raise Exception(error)  
        return error;
