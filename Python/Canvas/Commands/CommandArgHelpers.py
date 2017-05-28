#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Util, Commands as CppCommands 
from FabricEngine.Canvas.Application.FabricApplicationStates import *

class CommandArgHelpers:

    ### Global
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
                if CppCommands.CommandArgHelpers_Python._IsRTValScriptableCommand_Python(cmd):

                    # The input arg is a RTVal, cast it to JSON.
                    if CommandArgHelpers.__IsPyRTValArg(arg):
                        arg = arg.getJSONStr()

                    else:
                        rtValType = CppCommands.CommandArgHelpers_Python._GetRTValCommandArgType_Python(key, cmd)

                        #Unknown type, it's in JSON already
                        if rtValType != "RTVal":
                            isArgStr = rtValType == "String"
                            
                            if CppCommands.CommandArgHelpers_Python._IsPathValueCommandArg_Python(key, cmd):
                            
                                rtVal = None
                                if CommandArgHelpers.__IsPyStringArg(arg):
                                    if "<" in arg:
                                        arg = arg.replace("<", "")
                                        arg = arg.replace(">", "")
                                        rtVal = client.RT.types.PathValue(client.RT.types.String(arg))
                                        arg = Util.RTValUtil.toJSON(rtVal)   

                            # If the input arg is a string and the cmd arg is not, 
                            # assume the input arg is already the JSON. Otherwise 
                            # we create the JSON from the pyton arg.
                            elif not (CommandArgHelpers.__IsPyStringArg(arg) and not isArgStr):
                                castArg = True
                                # Check if the string has quotes.
                                # If so, we assume that it's already a JSON
                                if CommandArgHelpers.__IsPyStringArg(arg) and isArgStr and len(arg) > 0:
                                    first = arg[0] == "'" or arg[0] == "\"" 
                                    last =  arg[len(arg)-1] == "'" or arg[len(arg)-1] == "\"" 
                                    castArg = not first and not last
                                     
                                if castArg:
                                    pyRTValType = getattr(client.RT.types, rtValType)
                                    # Check if the python type can be casted as a RTVal.
                                    if pyRTValType().getSimpleType() is None:
                                        raise Exception("Can't cast python '" + str(type(arg)) + "' to rtVal '" + str(rtValType) + "'")
                                    # Construct the python RTVal and sets its arg.
                                    rtVal = pyRTValType(arg)   
                                    arg = Util.RTValUtil.toJSON(rtVal) 
        
                # CppCommands.BaseScriptableCommand, all in strings
                else:
                    if not CommandArgHelpers.__IsPyStringArg(arg):
                        raise Exception(
                            "CommandManagerQtCallback.__CastCmdArgsToStr, error: \n" + 
                            "The argument '" + str(key) + "' is not astring, but a " + str(type(arg)))

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
                    rtValType = CppCommands.CommandArgHelpers_Python._GetRTValCommandArgType_Python(key, cmd)
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
        if not CppCommands.CommandArgHelpers_Python._IsScriptableCommand_Python(cmd):
            raise Exception(
                "CommandManager.CommandArgHelpers.CastAndCheckCmdArgs, error: Command '" +
                str(cmd.getName()) + "' is not scriptable" )

        # If the command is a RTValScriptable and its RTVal args type have
        # been set, call checkRTValCommandArgs, checkCommandArgs otherwise.
        createRTValCommand = False
        for key, arg in args.iteritems():
            if CppCommands.CommandArgHelpers_Python._HasCommandArg_Python(key, cmd) is False:
                raise Exception(
                    "CommandManager.CommandArgHelpers.CastAndCheckCmdArgs, error: arg '" + 
                    str(key) + "' doesn't exist" )

            if CppCommands.CommandArgHelpers_Python._IsRTValScriptableCommand_Python(cmd):
                if CommandArgHelpers.__IsPyRTValArg(arg):
                    createRTValCommand = True
                    break

        if createRTValCommand:
            return True, CommandArgHelpers.CastCmdArgsToRTVal(cmd, args)
        else:
           return False, CommandArgHelpers.__CastCmdArgsToStr(cmd, args)


    ### CommandManagerQtCallback
    @staticmethod
    def __EncodeJSONChars(string):
        """ \internal """

        result = ""

        for ch in string:
            if ch == "\"":
                result += "'"

            elif ch == "\\":
                result += "\\\\"

            elif ch == "\r" or ch == "\n"  or ch == "\t":
                result += ""
                
            else:
                result += ch

        result = result.replace(" ", "")

        return result

    @staticmethod
    def __EncodeJSON(string):
        """ \internal """
        return "\"" + CommandArgHelpers.__EncodeJSONChars(string) + "\""

    @staticmethod
    def __EncodeJSONs(strings):
        """ \internal """
        result = "\""
        for i in range(0, len(strings)):
            if i > 0:
                result += "|"
            result += CommandArgHelpers.__EncodeJSONChars(strings[i])
        result += "\""
        return result

    @staticmethod
    def ParseCmdArgs(cmd):
        """ \internal, parses the command argumentss to construct
            a description of the command logged in the scrip-editor. 
        """

        desc = cmd.getName()

        if CppCommands.CommandArgHelpers_Python._IsScriptableCommand_Python(cmd):
            client = GetAppStates().getClient()

            try:
                keys = CppCommands.CommandArgHelpers_Python._GetCommandArgKeys_Python(cmd)
     
                desc += '('
                previous = ""
                count = 0
                
                for key in keys:

                    # Log only loggable commands.
                    if (CppCommands.CommandArgHelpers_Python._IsCommandArgSet_Python(key, cmd) and 
                        CppCommands.CommandArgHelpers_Python._IsCommandArg_Python(key, CppCommands.CommandArgFlags.LOGGABLE_ARG, cmd)):

                        if len(previous) > 0 and count < len(keys):
                            desc += ', '

                        desc += str(key) + '='
                        previous = str(key)

                        # RTValScriptableCommand, arguments are RTVal.
                        if CppCommands.CommandArgHelpers_Python._IsRTValScriptableCommand_Python(cmd):

                            rtVal = None
                          
                            if CppCommands.CommandArgHelpers_Python._IsPathValueCommandArg_Python(key, cmd):
                                pathValue = client.RT.types.PathValue(CppCommands.CommandArgHelpers_Python._GetRTValCommandArg_Python(key, "PathValue", cmd))
                                path = client.RT.types.String(pathValue.path)
                                if len(path.getSimpleType()) > 0:
                                    rtVal = path
                                else:
                                    rtVal = pathValue.value
                            else:
                                rtVal = CppCommands.CommandArgHelpers_Python._GetRTValCommandArg_Python(key, cmd)                        

                            pythonVal = rtVal.getSimpleType()
                           
                            # Can cast the RTVal in simple JSON type
                            if pythonVal is not None:
                                
                                rtValType = Util.RTValUtil.getType(rtVal)

                                if rtValType == 'String':
                                    str_ = rtVal.getSimpleType()
                                    if CppCommands.CommandArgHelpers_Python._IsPathValueCommandArg_Python(key, cmd):
                                        str_ = "<" + str_ + ">" 
                                    desc += "\"" + str_ + "\"" 
                                
                                else:
                                    desc += str(pythonVal)

                            # JSON
                            else:
                                desc += CommandArgHelpers.__EncodeJSON(str(Util.RTValUtil.toJSON(rtVal)))

                        # ScriptableCommand, arguments are strings.
                        else:
                            desc += str(CppCommands.CommandArgHelpers_Python._GetCommandArg_Python(key, cmd))
                   
                    count += 1   

                desc += ')'
 
            except Exception as e:    
                raise Exception('CommandManagerQtCallback.ParseCmdArgs, error: ' + str(e) )
            
        return desc
