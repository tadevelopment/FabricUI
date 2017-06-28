#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Util, Commands as CppCommands 
from FabricEngine.Canvas.Application.FabricApplicationStates import *

class CommandArgHelpers:

    @staticmethod
    def __IsPyStringArg(arg):
        """ \internal """
        return issubclass(type(arg), str) or issubclass(type(arg), unicode)

    @staticmethod       
    def __IsPyRTValArg(arg):
        """ \internal """
        # Construct a dumb RTVal to compare the python
        # object type --> find a better way.
        return type(arg) == type(GetAppStates().getClient().RT.types.UInt32())
 
    ### CommandManager
    @staticmethod
    def __CastCmdArgsToStr(cmd, args):
        """ \internal, casts the command's args into String. 
        """
        try:
            client = GetAppStates().getClient()
            strArgs = {}

            for key, arg in args.iteritems():

                # CppCommands.BaseRTValScriptableCommand
                if CppCommands.CommandArgHelpers.isRTValScriptableCommand(cmd):

                    # The input arg is a RTVal, cast it to JSON.
                    if CommandArgHelpers.__IsPyRTValArg(arg):
                        arg = arg.getJSONStr()

                    else:
                        rtValType = CppCommands.CommandArgHelpers.getRTValCommandArgType(key, cmd)
                        if rtValType == "None":
                            arg = None
                        else:
                            isArgStr = rtValType == "String"
                            isArgRTVal = rtValType == "RTVal"
 
                            # If the input arg is a string and the cmd arg is not, 
                            # assume the input arg is already the JSON. Otherwise 
                            # we create the JSON from the pyton arg.
                            if not isArgRTVal and not (CommandArgHelpers.__IsPyStringArg(arg) and not isArgStr):
                                castArg = True
                                # Check if the string has quotes.
                                # If so, we assume that it's already a JSON
                                if CommandArgHelpers.__IsPyStringArg(arg) and isArgStr and len(arg) > 0:
                                    first = arg[0] == "'" or arg[0] == "\"" 
                                    last = arg[len(arg)-1] == "'" or arg[len(arg)-1] == "\"" 
                                    castArg = not first and not last
                                 
                                if castArg:
                                    pyRTValType = getattr(client.RT.types, rtValType)
                                    # Check if the python type can be casted as a RTVal.
                                    if pyRTValType().getSimpleType() is None:
                                        raise Exception("Can't cast python '" + str(type(arg)) + "' to rtVal '" + str(rtValType) + "'")
                                    # Construct the python RTVal and sets its arg.
                                    rtVal = pyRTValType(arg)
                                    if rtVal is not None:
                                        arg = Util.RTValUtil.toJSON(rtVal) 
                                    else:
                                        arg = None
        
                # CppCommands.BaseScriptableCommand, all in strings
                elif CppCommands.CommandArgHelpers.isScriptableCommand(cmd):
                    if CommandArgHelpers.__IsPyRTValArg(arg):
                        pathValue = Util.RTValUtil.toRTVal(arg)
                        val = Util.RTValUtil.toRTVal(pathValue.value)
                        if CommandArgHelpers.__IsPyRTValArg(arg) and Util.RTValUtil.getType(val) == 'String':
                            arg = val.getSimpleType()
                    else:
                        arg = str(arg)

                if arg is not None:
                    strArgs[key] = arg
 
        except Exception as e:    
            raise Exception("CommandHelpers.__CastCmdArgsToStr, error: " + str(e) )
        
        return strArgs

    @staticmethod
    def CastCmdArgsToRTVal(cmd, args):
        """ \internal, casts the command's args into RTVal. 
        """
        try:
            rtValArgs = {}
            client = GetAppStates().getClient()

            for key, arg in args.iteritems():
            
                if not CommandArgHelpers.__IsPyRTValArg(arg):
                    rtValType = CppCommands.CommandArgHelpers.getRTValCommandArgType(key, cmd)
                    pyRTValType = getattr(client.RT.types, rtValType)
                
                    if CommandArgHelpers.__IsPyStringArg(arg) and rtValType != "String":
                        rtVal = pyRTValType()   
                        rtVal.setJSON(arg)  
                        arg = client.RT.types.RTVal(rtVal)                 
                    else:
                        arg = client.RT.types.RTVal(arg)  

                rtValArgs[key] = client.RT.types.RTVal(arg)    
                   
            return args
        
        except Exception as e:    
            raise Exception(
                'CommandHelpers.CastCmdArgsToRTVal, error: ' + str(e) )

    @staticmethod
    def CastAndCheckCmdArgs(cmd, args):
        """ \internal, casts the commands's args depending of their types.
        """
        if not CppCommands.CommandArgHelpers.isScriptableCommand(cmd):
            raise Exception(
                "CommandManager.CommandArgHelpers.CastAndCheckCmdArgs, error: Command '" +
                str(cmd.getName()) + "' is not scriptable" )

        # If the command is a RTValScriptable and its RTVal args type have
        # been set, call checkRTValCommandArgs, checkCommandArgs otherwise.
        isRTValCommand = False
        for key, arg in args.iteritems():
            if CppCommands.CommandArgHelpers.hasCommandArg(key, cmd) is False:
                raise Exception(
                    "CommandManager.CommandArgHelpers.CastAndCheckCmdArgs, error: arg '" + 
                    str(key) + "' doesn't exist" )

            if CppCommands.CommandArgHelpers.isRTValScriptableCommand(cmd):
                if CommandArgHelpers.__IsPyRTValArg(arg):
                    isRTValCommand = True
                    break

        if isRTValCommand:
            return True, CommandArgHelpers.CastCmdArgsToRTVal(cmd, args)
        else:
            return False, CommandArgHelpers.__CastCmdArgsToStr(cmd, args)

    ### CommandManagerQtCallback
    @staticmethod
    def ParseCmdArgs(cmd):
        """ \internal, parses the command arguments to construct
            a description of the command logged in the script-editor. 
        """
        desc = 'Commands.' + cmd.getName()
        
        if CppCommands.CommandArgHelpers.isScriptableCommand(cmd):
            try:
                keys = CppCommands.CommandArgHelpers.getCommandArgKeys(cmd)
                desc += '('
                first = True
                
                for key in keys:
                    # Log only loggable commands.
                    if (CppCommands.CommandArgHelpers.isCommandArgSet(key, cmd) and 
                        not CppCommands.CommandArgHelpers.isCommandArg(key, CppCommands.CommandArgFlags.DONT_LOG_ARG, cmd)):

                        argdesc = str(key) + '='

                        # RTValScriptableCommand, arguments are RTVal.
                        if CppCommands.CommandArgHelpers.isRTValScriptableCommand(cmd):
                            pathValue = CppCommands.CommandArgHelpers.getRTValCommandArg(key, cmd)

                            path = pathValue.path.getSimpleType()
                            if path:                       
                                argdesc += "\"" + "<" + path + ">" + "\"" 
                                
                            else:
                                rtVal = Util.RTValUtil.toRTVal(pathValue.value)
                                if rtVal is None:
                                    # Invalid RTVal - no value: don't log the arg'
                                    argdesc = ""
                                else:
                                    pythonVal = rtVal.getSimpleType()
                                   
                                    # Can cast the RTVal in simple JSON type
                                    if pythonVal is not None:
                                        if Util.RTValUtil.getType(rtVal) == 'String':
                                            argdesc += "\"" + pythonVal + "\"" 
                                        else:
                                            argdesc += str(pythonVal)
                                    # JSON
                                    else:
                                        argdesc += CppCommands.CommandArgHelpers.encodeJSON(str(Util.RTValUtil.toJSON(rtVal)))

                        # ScriptableCommand, arguments are Strings.
                        else:
                            argdesc += str(CppCommands.CommandArgHelpers.getCommandArg(key, cmd))
   
                        if argdesc:
                          if not first:
                            desc += ', '
                          first = False
                          desc += argdesc
 
                desc += ')'

            except Exception as e:    
                raise Exception('CommandManagerQtCallback.ParseCmdArgs, error: ' + str(e) )
            
        return desc
