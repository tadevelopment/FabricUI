#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui, QtCore
from FabricEngine.FabricUI import Commands
from FabricEngine.Canvas.Commands.CommandManager import *
 
class CommandManagerQtCallback(QtCore.QObject):

    """ CommandManagerQtCallback is connected to the CommandManagerCallback 
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

    def __onCommandPushedCallback(self, cmd):
        """ \internal
            Called when a command has been pushed to the manager. 
        """
        
        # Create the command wrapper
        oldEchoStackIndexChanges = self.scriptEditor._echoStackIndexChanges
        self.scriptEditor._echoStackIndexChanges = False

        self.qUndoStack.push( self.CommandQtWrapper( cmd.getName() ) )
        self.scriptEditor._echoStackIndexChanges = oldEchoStackIndexChanges

        # Log the command
        if issubclass(type(cmd), Commands.BaseScriptableCommand):
            self.scriptEditor.logText( cmd.getDescription() )
        else:
            self.scriptEditor.logText( cmd.getName() )

    def __onCleared(self):
        """ \internal
            Called when the manager is cleared. 
        """
        
        self.qUndoStack.clear()