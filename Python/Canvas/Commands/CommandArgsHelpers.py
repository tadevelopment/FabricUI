#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.FabricUI import Commands as CppCommands

class CommandArgsHelpers:

    ### Global
    @staticmethod
    def __IsPyStringArg(arg):
        """ \internal """
        return issubclass(type(arg), str) or issubclass(type(arg), unicode)

    @staticmethod
    def __IsPyRTValArg(client, arg):
        """ \internal """
        # Construct a dumb RTVal to compare the python
        # object type --> find a better way.
        return type(arg) == type(client.RT.types.UInt32())

    @staticmethod
    def IsRTValScriptableCmd(cmd):
        """ \internal """
        return issubclass(type(cmd), CppCommands.BaseRTValScriptableCommand)

    @staticmethod
    def IsScriptableCmd(cmd):
        """ \internal """
        return issubclass(type(cmd), CppCommands.BaseScriptableCommand)

    ### CommandManager
    @staticmethod
    def CastCmdArgsToStr(client, cmd, args):
        """ \internal, casts the command's args into String. 
        """

        try:
            strArgs = {}

            for key, arg in args.iteritems():
                 
                # CppCommands.BaseRTValScriptableCommand
                if CommandArgsHelpers.IsRTValScriptableCmd(cmd):

                    # The input arg is a RTVal, cast it to JSON.
                    if CommandArgsHelpers.__IsPyRTValArg(client, arg):
                        arg = arg.getJSONStr()

                    else:
                        # Get the KL RTVal type.
                        rtValType = cmd.getRTValArgType(key)
                        isArgStr = rtValType == "String"

                        # The arg is Python. If the input arg is a string and the
                        # cmd arg is not, assume the input arg is already the JSON. 
                        # Otherwise we create the JSON from the pyton arg.
                        if not (CommandArgsHelpers.__IsPyStringArg(arg) and not isArgStr):
                            castArg = True

                            # Check if the string has quotes.
                            # If so, we assume that it's already a JSON
                            if CommandArgsHelpers.__IsPyStringArg(arg) and isArgStr and len(arg) > 0:
                                first = arg[0] == "'" or arg[0] == "\"" 
                                last =  arg[len(arg)-1] == "'" or arg[len(arg)-1] == "\"" 
                                castArg = not first and not last

                            if castArg:
                                # Get the python RTVal object.
                                pyRTValType = getattr(client.RT.types, rtValType)

                                # Check if the python type can be casted as a RTVal.
                                if pyRTValType().getSimpleType() is None:
                                    raise Exception("Can't cast python '" + str(type(arg)) + 
                                        "' to rtVal '" + str(rtValType) + "'")
                                
                                # Construct the python RTVal and sets its arg.
                                rtVal = pyRTValType(arg)   
                                arg = rtVal.getJSONStr()   
                
                # CppCommands.BaseScriptableCommand, all in strings
                else:
                    if not CommandArgsHelpers.__IsPyStringArg(arg):
                        raise Exception(
                            "CommandManagerQtCallback.CastCmdArgsToStr, error: \n" + 
                            "The argument '" + str(key) + "' is not astring, but a " + str(type(arg)))

                strArgs[key] = arg
                               
        except Exception as e:    
            raise Exception("CommandHelpers.CastCmdArgsToStr, error: " + str(e) )
        
        return strArgs

    @staticmethod
    def CastCmdArgsToRTVal(client, cmd, args):
        """ \internal, casts the command's args into RTVal. 
        """
        try:
            rtValArgs = {}

            for key, arg in args.iteritems():
            
                if not CommandArgsHelpers.__IsPyRTValArg(client, arg):
                    rtValType = cmd.getRTValArgType(key)
                    pyRTValType = getattr(client.RT.types, rtValType)
                
                    if CommandArgsHelpers.__IsPyStringArg(arg) and rtValType != "String":
                        rtVal = pyRTValType()   
                        rtVal.setJSON(arg)  
                        arg = client.RT.types.RTVal(rtVal)                 
                     
                    else:
                        arg = client.RT.types.RTVal(arg)  

                rtValArgs[key] = client.RT.types.RTVal(arg)    
                   
            return rtValArgs
        
        except Exception as e:    
            raise Exception(
                'CommandHelpers.CastCmdArgsToRTVal, error: ' + str(e) )

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
        return "\"" + CommandArgsHelpers.__EncodeJSONChars(string) + "\""

    @staticmethod
    def __EncodeJSONs(strings):
        """ \internal """
        result = "\""
        for i in range(0, len(strings)):
            if i > 0:
                result += "|"
            result += CommandArgsHelpers.__EncodeJSONChars(strings[i])
        result += "\""
        return result

    @staticmethod
    def ParseCmdArgs(client, cmd):
        """ \internal, parses the command argumentss to construct
            a description of the command logged in the scrip-editor. 
        """

        desc = cmd.getName()
        if CommandArgsHelpers.IsScriptableCmd(cmd):

            try:
                keys = cmd.getArgKeys()
     
                desc += '('
                   
                count = 0
                for key in keys:

                    # Log only loggable commands.
                    if cmd.isArgSet(key) and cmd.isArg(key, CppCommands.CommandFlags.LOGGABLE_ARG):

                        if count > 0 and count < len(keys):
                            desc += ', '

                        desc += str(key) + '='

                        # RTValScriptableCommand, arguments are RTVal.
                        if CommandArgsHelpers.IsRTValScriptableCmd(cmd):

                            if len(cmd.getRTValArgType(key)) == 0:
                                raise Exception(
                                    "CommandManagerQtCallback.ParseCmdArgs, error: \n" + 
                                    "The argument '" + str(key) + "' is invalid" )

                            rtVal = cmd.getRTValArg(key)
                            pythonVal = rtVal.getSimpleType()

                            # Can cast the RTVal in simple JSON type
                            if pythonVal is not None:
                                rtValType = rtVal.type('String').getSimpleType()

                                if rtValType == 'String':
                                    pyRTValType = getattr(client.RT.types, rtValType)
                                    desc += str(pyRTValType(cmd.getArg(key)).getSimpleType())
                                else:
                                    desc += str(pythonVal)

                            # JSON
                            else:
                                desc += CommandArgsHelpers.__EncodeJSON(str(cmd.getArg(key)))

                        # ScriptableCommand, arguments are strings.
                        else:
                            desc += str(cmd.getArg(key))

                    count += 1   

                desc += ')'
 
            except Exception as e:    
                raise Exception('CommandManagerQtCallback.ParseCmdArgs, error: ' + str(e) )
            
        return desc
