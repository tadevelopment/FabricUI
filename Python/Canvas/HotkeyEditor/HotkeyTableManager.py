#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import json, os
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions
  
class HotkeyTableManager(QtCore.QObject):

    """ HotkeyTableManager. 
    """

    stateIsDirty = QtCore.Signal(bool, bool)
    updateShortcutItem = QtCore.Signal(str, str)

    def __init__(self, parentWidget, settings):
        super(HotkeyTableManager, self).__init__()

        self.parentWidget = parentWidget
        self.settings = settings
 
        # Lists of changed items. Use as a temp buffer so actions' 
        # shorcuts are not overrided when the user assign a new key.
        self.__changedActionDic = {}

        # shortcuts saving.
        self.filename = None
        self.jsonData = None

    def __reset(self):
        """ \internal.
        """
        self.__changedActionDic = {}
 
    def __emitStateIsDirty(self, dirty, clear):
        """ \internal.
        """
        self.stateIsDirty.emit(dirty, clear)
 
    def getShortcutFromJSON(self, actionName):
        """ \internal.
            Returns the shortcut in the json data
            from the command name.
        """
        if self.jsonData and actionName in self.jsonData:
            shortcutList = self.jsonData[actionName]
            if shortcutList:
                return shortcutList[0]

    def __checkIfSaveNeeded(self):
        """ \internal.
            Check if the changes needs to be saved.
        """

        if len(self.__changedActionDic) > 0:
            msgBox = QtGui.QMessageBox(
                QtGui.QMessageBox.Warning, 
                'Hotkey editor', 
                'Changes not saved',
                QtGui.QMessageBox.Yes | QtGui.QMessageBox.No | QtGui.QMessageBox.Cancel,
                self.parentWidget
                );
            msgBox.setInformativeText("Do you want to save your changes?")
            
            msgBox.setDefaultButton(QtGui.QMessageBox.Yes)
            ret = msgBox.exec_()

            if ret == QtGui.QMessageBox.Yes:
                self.saveActions()

            elif ret == QtGui.QMessageBox.Cancel:
                return True

        return False

    def __setItemKeySequenceAndShortcut(self, actionName, keySequence = None, shortcut = None):
        """ \internal.
        """
        
        if shortcut:
            self.__changedActionDic[actionName] = keySequence
            self.updateShortcutItem.emit(actionName, shortcut)
        
        else:
            self.__changedActionDic[actionName] = QtGui.QKeySequence()
            self.updateShortcutItem.emit(actionName, '')

    def setItemKeySequence(self, actionName, keySequence, force = False, isDirty = True):
        """ \internal.
            Sets the keySequence to the item. If another items/actions 
            used the keySequence already, a warning is displayed and 
            the method returns. if 'force' is true, force to setting.

            If a keySequence is invalid or corresponds the `Del` or 
            `Backspace` key, the item keySequence is invalidate.
        """
         
        if  (  keySequence != QtGui.QKeySequence() and 
                keySequence != QtGui.QKeySequence('Del') and 
                keySequence != QtGui.QKeySequence('Backspace') ):

            shortcut = keySequence.toString(
                QtGui.QKeySequence.NativeText
                )

            actionName_ = self.__shortcutIsUsedBy(
                actionName, 
                keySequence
                )
                
            if actionName_ and not force:
                message = QtGui.QMessageBox()
                message.warning(
                    self.parentWidget, 
                    'Hotkey editor', 
                    'shorcut ' + str(shortcut) + ' already used by ' + str(actionName_)
                    )

                return False

            elif not actionName_ or force:
                self.__setItemKeySequenceAndShortcut(
                    actionName, 
                    keySequence, 
                    shortcut)
        
        else:
            self.__setItemKeySequenceAndShortcut(
                actionName)

        self.__emitStateIsDirty(
            isDirty,
            False)

        return True

    def isActionContextGlobal(self, action):
        """ \internal.
            Check if the action' context is global:
            WindowShortcut or ApplicationShortcut. 
        """
        return (action.shortcutContext() == QtCore.Qt.WindowShortcut or  
                action.shortcutContext() == QtCore.Qt.ApplicationShortcut) 

    def __shortcutIsUsedBy(self, actionName, shortcut):
        """ \internal.
            Check if the action named `actionName` can use the shorcut.
            If true, returns None. Otherwise, returns the name of the 
            action using the short cut alredy.

            Arguments:
            - actionName: The action that wants to use the shorcut.
            - shortcut: The shortcut to use.
        """
 
        actionRegistry = Actions.ActionRegistry.GetActionRegistry()
        otherActionList = actionRegistry.isShortcutUsed(shortcut)

        for otherActionName, shortcut_ in self.__changedActionDic.iteritems():
            if shortcut_ == shortcut:
                otherActionList.append(actionRegistry.getAction(otherActionName))

        action = actionRegistry.getAction(actionName)
        isActionContextGlobal = self.isActionContextGlobal(action) 

        if action in otherActionList:
            otherActionList.remove(action)

        for otherAction in otherActionList:
            
            otherActionName = actionRegistry.getActionName(otherAction)
            isOtherActionContextGlobal = self.isActionContextGlobal(otherAction) 
 
            # If both actions are local (to their widget),
            # check if they have the same parent.
            sharedSameParent = False
            if not isOtherActionContextGlobal and not isActionContextGlobal:
                sharedSameParent = otherAction.parentWidget() == action.parentWidget()
  
            if (isOtherActionContextGlobal and isActionContextGlobal) or sharedSameParent:
                return otherActionName
 
    def acceptShortcutChanges(self, isDirty = True, clear = False):
        """ \internal.
            Accepts the changes. Sets the actions shortcuts 
            from the items text.
        """

        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        for actionName, keySequence in self.__changedActionDic.iteritems():
            # Set the same shortcut to all the  
            # actions registered as `actionName`.
            actionRegistry.setShortcut(actionName, keySequence)

        self.__reset()
        self.__emitStateIsDirty(isDirty, clear)

    def rejectShortcutChanges(self):
        """ \internal.
            Rejects the changes. Sets the items text from
            the actions shortcuts. 
        """

        hasChanged = len(self.__changedActionDic) > 0

        if hasChanged:
            actionRegistry = Actions.ActionRegistry.GetActionRegistry()

            for actionName in self.__changedActionDic:
                keySequence = actionRegistry.getShortcut(actionName)

                self.setItemKeySequence(
                    actionName, 
                    keySequence, 
                    True)
 
            self.__reset()
 
    def openActions(self):
        """ \internal.
            Opens a json file containing a list of pair {actionName, shortcut}
            that override the current actions' shortcuts (if matched). Report 
            a warning if changes have niot been saved.
        """
        if self.__checkIfSaveNeeded():
            return False

        lastDir = str(self.settings.value("hotkeyEditor/lastFolder"))
        fname, _ = QtGui.QFileDialog.getOpenFileName(self.parentWidget, "Open Hotkey file", lastDir, "*.json")

        fname = str(fname)
        if len(fname) == 0:
            return False

        else:
            # Sets the current file
            self.filename = fname

            # Override the shortcuts 
            # from the json matches.
            self.jsonData = None
            with open(self.filename) as infile:  
                self.jsonData = json.load(infile)

            for actionName, shortcutList in self.jsonData.iteritems():
                 
                shortcut = ''    
                if shortcutList:
                    shortcut = shortcutList[0]
                    
                    self.setItemKeySequence(
                        actionName, 
                        QtGui.QKeySequence(shortcut), 
                        True)

            self.acceptShortcutChanges(False, True)
            return True

    def saveActions(self):
        """ \internal.
            Saves in the current json file a list of pair 
            {actionName, shortcut} of the current actions. 
        """
        # If no filename, call save as.
        if not self.filename and not self.saveActionsAs():
            return

        self.acceptShortcutChanges(False, True)

        actionRegistry = Actions.ActionRegistry.GetActionRegistry()

        self.jsonData = {}
        for actionName in actionRegistry.getActionNameList():
            keySequenceList = actionRegistry.getShortcuts(actionName)
            
            keyStringSequenceList = []
            for keySequence in keySequenceList:
                keyStringSequenceList.append(keySequence.toString(QtGui.QKeySequence.NativeText))
            
            self.jsonData[actionName] = keyStringSequenceList
         
        with open(self.filename, 'w') as outfile:  
            json.dump(self.jsonData, outfile, ensure_ascii=False, indent=4)

    def saveActionsAs(self):
        """ \internal.
            Saves in a new json file a list of pair 
            {actionName, shortcut} of the current actions. 
        """

        ext = ".json"
        fname = self.filename
        
        if not fname:
            fname = str(self.settings.value("hotkeyEditor/lastFolder"))
        fname, _ = QtGui.QFileDialog.getSaveFileName(self.parentWidget, "Save Hotkey file", fname, str("*" + ext))
            
        if not fname:
            return False

        # Pyside QFileDialog bug on linux.
        # Extension is not added by default.
        if not fname.endswith(ext):
            fname += ext

        self.settings.setValue("hotkeyEditor/lastFolder", os.path.dirname(fname))
        self.filename = fname

        self.saveActions()
 