#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import os
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Commands, Util
from FabricEngine.Canvas.Dialogs.HelpDialog import HelpDialog
 
class OpenHelpDialogCommand(Commands.BaseScriptableCommand):

    def __init__(self):
        super(OpenHelpDialogCommand, self).__init__()
        self.declareArg("title", False, "")
        self.declareArg("text", False, "")
 
    def canUndo(self):
        """ \internal, impl. of BaseCommand. 
        """ 
        return False

    def canLog(self):
        """ \internal, impl. of BaseCommand. 
        """
        return False

    def doIt(self):
        """ \internal, impl. of BaseCommand. 
        """
        mainWIndow = Util.QtUtil.getMainWindow()

        dialog = HelpDialog(
            self.getArg("title"), 
            self.getArg("text"), 
            mainWIndow)

        pos = QtGui.QCursor.pos()
        dialog.move(pos.x(), pos.y())

        dialog.show()
        return True

    def getHelp(self):
        """ \internal, impl. of BaseCommand. 
        """
        argsHelp = {
            "title" : "Title of the Dialog widget",
            "text " : "Text to display"
        }
        return Commands.CommandArgHelpers.createHelpFromArgs(
            "Open a Dialog placed at the current mouse position",
            argsHelp,
            self);
