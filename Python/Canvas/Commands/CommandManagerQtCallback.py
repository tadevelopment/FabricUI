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
                # 'redo' is called when the Qt command wrapper 
                # is pushed to the Qt stack. Because the command 
                # http://doc.qt.io/qt-4.8/qundostack.html#push
                # We deactivate it.
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
        GetCommandManager().commandPushedCallback.connect(self.__onCommandPushedCallback)
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

    def __getCommandDescription(self, cmd):
        """ \internal
            Gets a description of the command,
            logged in the scrip-editor. 
        """

        desc = cmd.getName()

        # Check if it's a BaseRTValScriptableCommand.
        if issubclass(type(cmd), Commands.BaseScriptableCommand):
            try:
                args = cmd.getArgs()
     
                # Check if it's a BaseRTValScriptableCommand.
                isRTValScriptCommand = issubclass(type(cmd), Commands.BaseRTValScriptableCommand)

                desc += "("
                    
                count = 0
                for key, value in args.iteritems():
                    desc += str(key) 
                    desc += "="

                    # BaseScriptableCommand.
                    # All arguments are strings.
                    if not isRTValScriptCommand:
                        desc += str(value)

                    # BaseRTValScriptableCommand.
                    else:
                        # Get the KL RTVal type.
                        rtValType = cmd.getArgType(key)

                        # Get the python RTVal object.
                        pyRTValType = getattr(GetCommandManager().client.RT.types, rtValType)

                        if pyRTValType == "String":
                            desc += str(pyRTValType(value).getSimpleType())

                        # Check if the RTVal type can be casted in Python.
                        elif pyRTValType().getSimpleType() is not None:
                            value = cmd.getArgAsRTVal(key)
                            desc += str(pyRTValType(value).getSimpleType())
                            
                        # JSON
                        else:
                            desc += self.__encodeJSON(str(value))

                    if count < len(args) - 1:
                        desc += ", "
                        count += 1   

                desc += ")"

                             
            except Exception as e:    
                raise Exception("CommandManagerQtCallback.__getCommandDescription, error: " + str(e) )
        
        return desc

    def __onCommandPushedCallback(self, cmd):
        """ \internal
            Called when a command has been pushed to the manager. 
        """
        
        # Create the command wrapper.
        oldEchoStackIndexChanges = self.scriptEditor._echoStackIndexChanges
        self.scriptEditor._echoStackIndexChanges = False
        self.qUndoStack.push( self.CommandQtWrapper( cmd.getName() ) )
        self.scriptEditor._echoStackIndexChanges = oldEchoStackIndexChanges

        # Log the commands.
        self.scriptEditor.logText( 
            "Commands." + self.__getCommandDescription(cmd) )

    def __onCleared(self):
        """ \internal
            Called when the manager is cleared. 
        """
        
        self.qUndoStack.clear()