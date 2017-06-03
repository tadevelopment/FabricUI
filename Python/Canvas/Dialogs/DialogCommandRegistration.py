#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from FabricEngine.Canvas.Commands.CommandRegistry import *
from FabricEngine.Canvas.Dialogs.OpenHelpDialogCommand import OpenHelpDialogCommand

class DialogCommandRegistration():

    @staticmethod
    def RegisterCommands():
    	GetCmdRegistry().registerCmd('openHelpDialog', OpenHelpDialogCommand)
        