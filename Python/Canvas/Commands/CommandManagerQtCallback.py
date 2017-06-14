#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui, QtCore
from FabricEngine.Canvas.Commands.CommandManager import *
from FabricEngine.Canvas.Commands.CommandArgHelpers import CommandArgHelpers

class CommandManagerQtCallback(QtCore.QObject):
    """ CommandManagerQtCallback is connected to the CommandManagerCallback 
        and adds the commands into the Qt undo stack when they are created. 
        When a command is added, it also logs its description automatically
        generated from the command name and arguments.
    """

    class CommandQtWrapper(QtGui.QUndoCommand):
        """ CommandQtWrapper wraps the commands so they are can be
            added to the Qt undo stack. The commands don't undo -
            redo them-selves, but ask the command manager to do it.
        """
        def __init__(self, name):
            QtGui.QUndoCommand.__init__(self)
            self.setText(name)
            self.canRedo = False

        def undo(self):
            """ Implementation of QUndoCommand
            """
            try:
                GetCommandManager().undoCommand()
            except Exception as e:    
                print str(e)

        def redo(self):
            """ Implementation of QUndoCommand
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
        super(CommandManagerQtCallback, self).__init__()
        self.qUndoStack = qUndoStack
        self.scriptEditor = scriptEditor
        GetCommandManager().cleared.connect(self.__onCleared)
        GetCommandManager().commandDone.connect(self.__onCommandDone)

    def __onCommandDone(self, cmd, pushedToStack, replace):
        """ \internal, when a command's been pushed to the manager. 
        """ 
        try:
            # Create a new CommandQtWrapper and  
            # pushs it to the qt undo stack.
            if pushedToStack:
                oldEchoStackIndexChanges = self.scriptEditor._echoStackIndexChanges
                self.scriptEditor._echoStackIndexChanges = False
                self.qUndoStack.push( self.CommandQtWrapper( cmd.getHistoryDesc() ) )
                self.scriptEditor._echoStackIndexChanges = oldEchoStackIndexChanges

            #Log the commands.
            if cmd.canLog():
                self.scriptEditor.logCommand(CommandArgHelpers.ParseCmdArgs(cmd),replace)
            
        except Exception as e:    
                print str(e)

    def __onCleared(self):
        """ \internal, called when the manager is cleared. 
        """
        self.qUndoStack.clear()