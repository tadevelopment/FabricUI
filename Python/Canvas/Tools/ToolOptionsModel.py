#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine import Core
from FabricEngine.Canvas.Utils import *
from FabricEngine.FabricUI import Commands, ValueEditor
from FabricEngine.Canvas.Commands.CommandRegistry import *

class ToolOptionsModel(ValueEditor.BaseModelItem):

    """ ToolOptionsModel is the model for a single tool's option.
    """

    def __init__(self, name, value, settings, namePath, editor):
        """ Initializes the ToolOptionsModel.
            
            Arguments:
            - name: A reference to the parent widget.
            - value: A reference the canvasWindow.
            - settings: A reference the canvasWindow.
            - namePath: A reference the canvasWindow.
            - editor: A reference the canvasWindow.
        """
        super(ToolOptionsModel, self).__init__()

        self.name = name
        self.value = value
        self.originalValue = value.clone()
        self.settings = settings
        self.namePath = namePath + '/' + name
        self.editor = editor

    def getValue(self):
        """ Implementation of ValueEditor.BaseModelItem.
        """
        return self.toVariant(
            self.value.clone()
            )
  
    def setValue(self, value):
        """ Implementation of ValueEditor.BaseModelItem.
        """
        # RTVariant::toRTVal might change the pointer in ioVal,
        # so we copy it to make sure that self.value will always point to the same place
        valCopy = self.value.clone()

        ValueEditor.RTVariant.toRTVal( 
            value, 
            valCopy 
            )

        self.value.assign( 
            valCopy 
            )

        # Storing the value in the Settings
        # m_settings.setValue( 
        #     m_namePath.data(), 
        #     QString( self.value.getJSON().getSimpleType() )
        #     )

        # Updating the UI
        self.editor.valueChanged.emit()
        self.emitModelValueChanged( self.getValue() )

    def setValue(self, value, commit, valueAtInteractionBegin):
 
        # might be invalid when changing a Float with the keyboard (as text), for example
        prevValue = None
        if valueAtInteractionBegin.isValid():
            prevValue = self.getValue()

        self.setValue( value )

        if( commit ):
            pass
          # self.editor.m_undoStack.push(
          #   new OptionUndoCommand(
          #     "Changed the option \"" + QString::fromUtf8( m_namePath.data(), m_namePath.size() ) + "\"",
          #     prevValue,
          #     value,
          #     *this
          #   )
          # )  

    def hasDefault(self): 
        """ Implementation of ValueEditor.BaseModelItem.
        """
        return True

    def resetToDefault(self):
        """ Implementation of ValueEditor.BaseModelItem.
        """
        setValue( 
            toVariant(m_originalValue), 
            true, 
            getValue() 
            )
     
    def getName(self):
        """ Implementation of ValueEditor.BaseModelItem.
        """
        return self.name


class ToolOptionsDictModel(ValueEditor.BaseModelItem):

    """ ToolOptionsDictModel  
    """

    def __init__(self, name, dictionary, settings, namePath, editor):
        """ Initializes the ToolOptionsDictModel.
            
            Arguments:
            - name: A reference to the parent widget.
            - dictionary: A reference to the parent widget.
            - settings: A reference to the parent widget.
            - namePath: A reference to the parent widget.
            - editor: A reference to the parent widget.
        """
        super(ToolOptionsDictModel, self).__init__()

        self.name = name
        self.namePath = namePath + '/' + name

        keys = dictionary.getDictKeys()
        for i in range(0, keys.getArraySize()):

            key = keys.getArrayElementRef(i)
            value = dictionary.getDictElement(key)
            if (value.isWrappedRTVal()):
                value = value.getUnwrappedRTVal() 

            item = None
            if value.isDict() and value.dictKeyHasType("String") and value.dictValueHasType("RTVal"):
                item = ToolOptionsModel(
                    key.getSimpleType(),
                    value,
                    settings,
                    self.namePath,
                    editor)
            else
                item = new ViewportOptionModel(
                    key.getStringCString(),
                    value,
                    settings,
                    m_namePath,
                    editor
                 
        # qss
        self.setObjectName('ToolOptionsDictModel')

    def onUpdateOptions(self):
        pass

 