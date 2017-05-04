#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui, QtCore
from FabricEngine.FabricUI import Commands
from FabricEngine.Canvas.Commands.CommandManager import *
 
class CommandManagerQtCallback(QtCore.QObject):

    """ CommandManagerQtCallback specializes CommandManagerCallback (C++) 
        and adds the commands into the Qt undo stack when they are created. 
        When a command is added, it also logs its description.
    """

    class CommandQtWrapper(QtGui.QUndoCommand):
        
        """ CommandQtWrapper wraps the commands so they are can be
            added to the Qt undo stack. The commands don't undo -
            redo them-selves, but ask the command manager to do it.
        """

        def __init__(self, name):
            """ Initializes the CommandQtWrapper.

                Arguments:
                - name: The command name.
            """
            QtGui.QUndoCommand.__init__(self)
            self.setText(name)
            self.canRedo = False

        def undo(self):
            """ Implmentation of QUndoCommand
            """
            try:
                GetCommandManager().undoCommand()
            except Exception as e:    
                print str(e)

        def redo(self):
            """ Implmentation of QUndoCommand
            """
            try:
                # 'redo' is called when a Qt command is pushed to the 
                #  stack. http://doc.qt.io/qt-4.8/qundostack.html#push.
                if self.canRedo is True:
                    GetCommandManager().redoCommand()
                else:
                    self.canRedo = True

            except Exception as e:    
                print str(e)

    def __init__(self, qUndoStack, scriptEditor):
        """ Initializes the CommandManagerQtCallback.

            Arguments:
            - qUndoStack: A reference to a Qt undo stack.
            - scriptEditor: A reference to the scriptEditor
        """

        super(CommandManagerQtCallback, self).__init__()
        self.qUndoStack = qUndoStack
        self.scriptEditor = scriptEditor
        GetCommandManager()._commandPushed_Python.connect(self.__onCommandPushed)
        GetCommandManager().cleared.connect(self.__onCleared)

    def __encodeJSONChars(self, string):

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
    
    def __encodeJSON(self, string):
        return "\"" + self.__encodeJSONChars(string) + "\""

    def __encodeJSONs(self, strings):
        result = "\""
        for i in range(0, len(strings)):
            if i > 0:
                result += "|"
            result += self.__encodeJSONChars(strings[i])
        result += "\""
        return result

    def __getCommandDescription(self, baseCmd):
        """ \internal
            Gets a description of the command,
            logged in the scrip-editor. 
        """

        desc = baseCmd.getName()

        if issubclass(type(baseCmd), Commands.BaseScriptableCommand):
            
            try:
                keys = baseCmd.getArgKeys()
     
                desc += '('
                    
                count = 0
                for key in keys:
                    desc += str(key) + '='

                    # RTValScriptableCommand, arguments are RTVal.
                    if issubclass(type(baseCmd), Commands.BaseRTValScriptableCommand):

                        rtVal = baseCmd.getRTValArg(key)
                        pythonVal = rtVal.getSimpleType()
                            
                        # Can cast the RTVal in simple JSON type
                        if pythonVal is not None:
                            rtValType = rtVal.type('String').getSimpleType()

                            if rtValType == 'String':
                                pyRTValType = getattr(GetCommandManager().getClient().RT.types, rtValType)
                                desc += str(pyRTValType(baseCmd.getArg(key)).getSimpleType())
                            else:
                                desc += str(rtVal.getSimpleType())

                        # JSON
                        else:
                            desc += self.__encodeJSON(str(baseCmd.getArg(key)))

                    # ScriptableCommand, arguments are strings.
                    else:
                        desc += str(baseCmd.getArg(key))

                    if count < len(keys) - 1:
                        desc += ', '
                        count += 1   

                desc += ')'
                    
            except Exception as e:    
                raise Exception('CommandManagerQtCallback.__getCommandDescription, error: ' + str(e) )
        
        return desc

    def __onCommandPushed(self, baseCmd):
        """ \internal
            Called when a command has been pushed to the manager. 
        """
        
        # Create the command wrapper.
        oldEchoStackIndexChanges = self.scriptEditor._echoStackIndexChanges
        self.scriptEditor._echoStackIndexChanges = False
        self.qUndoStack.push( self.CommandQtWrapper( baseCmd.getName() ) )
        self.scriptEditor._echoStackIndexChanges = oldEchoStackIndexChanges

        # Log the commands.
        self.scriptEditor.logText( 
            'Commands.' + self.__getCommandDescription(baseCmd) )

    def __onCleared(self):
        """ \internal
            Called when the manager is cleared. 
        """
        self.qUndoStack.clear()