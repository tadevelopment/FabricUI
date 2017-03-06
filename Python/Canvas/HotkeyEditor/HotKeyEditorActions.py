#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import os
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions
    
class BaseHotkeyEditorAction(Actions.BaseAction):
 
    def __init__(self, hotkeyEditor, name, text, shortcut):

        self.hotkeyEditor = hotkeyEditor

        super(BaseHotkeyEditorAction, self).__init__(
            hotkeyEditor, 
            name, 
            text, 
            shortcut, 
            QtCore.Qt.ApplicationShortcut)
 
class OpenHotkeyFileAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(OpenHotkeyFileAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.OpenHotkeyFileAction", 
            "Open", 
            QtGui.QKeySequence(""))

        self.setToolTip('Open a Hotkey file')
        
    def onTriggered(self):
        if self.hotkeyEditor.hotkeyTable.openActions():
            basename = os.path.basename(self.hotkeyEditor.hotkeyTable.filename)
            self.hotkeyEditor.setWindowTitle('Hotkey Editor: ' + basename)

class SaveHotkeyFileAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(SaveHotkeyFileAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.SaveHotkeyFileAction", 
            "Save", 
            QtGui.QKeySequence(""))

        self.setToolTip('Save a Hotkey file')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.saveActions()
 
class SaveHotkeyFileAsAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(SaveHotkeyFileAsAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.SaveHotkeyFileAsAction", 
            "Save As", 
            QtGui.QKeySequence(""))

        self.setToolTip('Save a Hotkey file as')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.saveActionsAs()
 
class AcceptActionChanges(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(AcceptActionChanges, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.AcceptActionChanges", 
            "Ok", 
            QtGui.QKeySequence(""))

        self.setToolTip('Accept a Hotkey changes')
        
    def onTriggered(self):
        self.hotkeyEditor.accept()

class RejectActionChanges(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(RejectActionChanges, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.RejectActionChanges", 
            "Cancel", 
            QtGui.QKeySequence(""))

        self.setToolTip('Reject a Hotkey changes')
        
    def onTriggered(self):
        self.hotkeyEditor.reject()
