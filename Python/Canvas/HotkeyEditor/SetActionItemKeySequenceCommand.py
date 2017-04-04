#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
 
class SetActionItemKeySequenceCommand(QtGui.QUndoCommand):
    """ SetActionItemKeySequenceCommand.
    """

    class State:
        REDO_IT = 0
        UNDO_IT = 1

    def __init__(self, qUndoStack, manager, actionName, prevKeySequence, keySequence):
        """ Initializes a SetActionItemKeySequenceCommand.

            Arguments:
            - manager: Reference to the HotkeyTableWidget.
            - actionName: Referemce to a ActionTableWidgetItem.
            - keySequence: The new QKeySequece.

        """
        super(SetActionItemKeySequenceCommand, self).__init__()

        self.qUndoStack = qUndoStack
        self.state = self.State.UNDO_IT

        self.actionName = actionName
        self.manager = manager;
        self.keySequence = keySequence
        self.prevKeySequence = prevKeySequence

    def doIt(self):
        """ Executes the command.
        """
        self.state = self.State.UNDO_IT

        return self.manager.setItemKeySequence(
            self.actionName, 
            self.keySequence
            )

    def redo(self):
        """ Implmentation of QUndoCommand.
        """
        if self.state == self.State.REDO_IT:
            self.doIt()
            self.state = self.State.UNDO_IT

    def undo(self):
        """ Implmentation of QUndoCommand.
        """

        if self.state == self.State.UNDO_IT:
            self.manager.setItemKeySequence(
                self.actionName, 
                self.prevKeySequence,
                False,
                self.qUndoStack.index() != 1)
            self.state = self.State.REDO_IT
