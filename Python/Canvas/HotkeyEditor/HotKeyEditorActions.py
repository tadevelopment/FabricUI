#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions
    
class BaseHotkeyEditorAction(Actions.BaseAction):
 
    def __init__(self, hotkeyEditor, name, text, shortcut):

        super(BaseHotkeyEditorAction, self).__init__(hotkeyEditor)
            
        self.name = name
        self.hotkeyEditor = hotkeyEditor
        self.hotkeyEditor.addAction(self)
        self.hotkeyEditor.hotkeyTable.editingItem.connect(self.__onEditingItem)

        super(BaseHotkeyEditorAction, self).init(
            name, 
            text, 
            shortcut, 
            QtCore.Qt.WidgetWithChildrenShortcut,
            True,
            False)  

    def __onEditingItem(self, editing):
        self.setEnabled(not editing)

class OpenHotkeyFileAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(OpenHotkeyFileAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.OpenHotkeyFileAction", 
            "Open", 
            QtGui.QKeySequence("Ctrl+O"))

        self.setToolTip('Open file')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.manager.openActions()
 
class SaveHotkeyFileAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(SaveHotkeyFileAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.SaveHotkeyFileAction", 
            "Save", 
            QtGui.QKeySequence("Ctrl+S"))

        self.setToolTip('Save file')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.manager.saveActions()
 
class SaveHotkeyFileAsAction(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(SaveHotkeyFileAsAction, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.SaveHotkeyFileAsAction", 
            "Save As", 
            QtGui.QKeySequence("Ctrl+Shift+S"))

        self.setToolTip('Save file as')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.manager.saveActionsAs()
 
class AcceptActionChangesAndExit(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(AcceptActionChangesAndExit, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.AcceptActionChangesAndExit", 
            "Ok", 
            QtGui.QKeySequence("Return"))

        self.setToolTip('Accept the changes and close the dialog')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.manager.acceptShortcutChanges()
        self.hotkeyEditor.accept()
 
class RejectActionChangesAndExit(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(RejectActionChangesAndExit, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.RejectActionChangesAndExit", 
            "Cancel", 
            QtGui.QKeySequence("Escape"))

        self.setToolTip('Reject the changes and close the dialog')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.manager.rejectShortcutChanges()
        self.hotkeyEditor.reject()

class UndoActionChanges(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(UndoActionChanges, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.UndoActionChanges", 
            "Cancel", 
            QtGui.QKeySequence("Ctrl+Z"))

        self.setToolTip('Undo the changes.')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.qUndoStack.undo()
 
class RedoActionChanges(BaseHotkeyEditorAction):
 
    def __init__(self, hotkeyEditor):
        super(RedoActionChanges, self).__init__(
            hotkeyEditor, 
            "HotkeyEditor.RedoActionChanges", 
            "Cancel", 
            QtGui.QKeySequence("Ctrl+Shift+Z"))

        self.setToolTip('Redo the changes.')
        
    def onTriggered(self):
        self.hotkeyEditor.hotkeyTable.qUndoStack.redo()
 