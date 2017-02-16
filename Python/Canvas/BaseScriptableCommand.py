#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands
  
class BaseScriptableCommand(Commands.BaseScriptableCommand_Python):
    
    """ BaseScriptableCommand is the Python version of the C++ class Commands/BaseScriptableCommand
        and specializes BaseScriptableCommand_Python.
        See Commands/BaseScriptableCommand.h(cpp) and Commands/BaseScriptableCommand_Python.h(cpp).

        All ScriptableCommand must derived from BaseScriptableCommand, 
        not from Commands.BaseScriptableCommand_Python
    """

    def __init__(self):
        super(BaseScriptableCommand, self).__init__()
 
    def declareArg(self, key, optional = True, defaultValue = ""):
        """ Implementation of BaseScriptableCommand.
            Raises an exception if an error occurs. 
        """
        error = super(BaseScriptableCommand, self).declareArg_Python(key, optional, defaultValue)
        if error:
            raise Exception(error)

 