#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtCore, QtGui
from FabricEngine.FabricUI import Actions as CppActions
#from FabricEngine.Canvas.Application.FabricApplicationStates import *
 
class HotkeyTableModel(QtCore.QObject):

    """ HotkeyTableModel. 
    """

    updateShortcutItem = QtCore.Signal(str, str)

    def __init__(self, parentWidget):
        super(HotkeyTableModel, self).__init__()
        self.parentWidget = parentWidget

    def __setItemKeySequenceAndShortcut(self, actionName, keySeq = QtGui.QKeySequence(), shortcut = ''):
        """ \internal.
        """
        CppActions.ActionRegistry.GetActionRegistry().setShortcut(actionName, keySeq)
        self.updateShortcutItem.emit(actionName, shortcut)

    def __shortcutIsUsedBy(self, actionName, shortcut):
        """ \internal.
            Check if the action named `actionName` can use the shorcut.
            If true, returns None. Otherwise, returns the name of the 
            action using the short cut alredy.
        """
        actRegistry = CppActions.ActionRegistry.GetActionRegistry()
        otherActionList = actRegistry.isShortcutUsed(shortcut)
        globalContext = actRegistry.isActionContextGlobal(actionName) 

        action = actRegistry.getAction(actionName)
        if action is None:
            return

        if action in otherActionList:
            otherActionList.remove(action)

        for otherAction in otherActionList:
            otherActionName = actRegistry.getActionName(otherAction)
            otherGlobalContext = actRegistry.isActionContextGlobal(otherActionName) 
            
            # If both actions are local (to their widget),
            # check if they have the same parent.
            sharedSameParent = False
            if not otherGlobalContext and not globalContext:
                sharedSameParent = otherAction.parentWidget() == action.parentWidget()

            if otherGlobalContext or globalContext or sharedSameParent:
                return otherActionName
 
    def setItemKeySequence(self, actionName, keySeq, force = True):
        """ \internal.
            Sets the keySeq to the item. If another items/actions 
            used the keySeq already, a warning is displayed and 
            the method returns. if 'force' is true, force to setting.

            If a keySeq is invalid or corresponds the `Del` or 
            `Backspace` key, the item keySeq is invalidate.
        """
        if  (  keySeq != QtGui.QKeySequence() and 
                keySeq != QtGui.QKeySequence('Del') and 
                keySeq != QtGui.QKeySequence('Backspace') ):

            shortcut = keySeq.toString(QtGui.QKeySequence.NativeText)
            actionName_ = self.__shortcutIsUsedBy(actionName, keySeq)
                
            if actionName_ and not force:
                message = QtGui.QMessageBox()
                message.warning(self.parentWidget, 'Hotkey editor', 
                    'shorcut ' + str(shortcut) + ' already used by ' + str(actionName_))
                return False

            elif not actionName_ or force:
                self.__setItemKeySequenceAndShortcut(actionName, keySeq, shortcut)
        
        else:
            self.__setItemKeySequenceAndShortcut(actionName)

        return True
