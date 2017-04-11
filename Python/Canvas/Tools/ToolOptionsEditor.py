#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine.FabricUI import OptionsEditor
 
class ToolOptionsEditor(OptionsEditor.BaseOptionsEditor):

    def __init__(self, tool, undoStack, settings):
        """ Initializes the ToolOptionsEditor.
        """

        super(ToolOptionsEditor, self).__init__(undoStack) 
        self.settings = settings
        self.tool = tool
        self.updateOptions()

    def updateOptions(self):
        super(ToolOptionsEditor, self).updateOptions() 

        print "updateOptions"
        dict_ = self.tool.getParams('RTVal[String]')

        self.m_model = OptionsEditor.OptionsDictModel(
            "Tool Options",
            dict_,
            self.settings,
            "",
            self
          )

        self.onSetModelItem(self.m_model);

class ToolOptionsEditorDialog(QtGui.QDialog):

    def __init__(self, parent, tool, undoStack, settings):
        """ Initializes the ToolOptionsEditorDialog.
        """

        super(ToolOptionsEditorDialog, self).__init__(parent) 

        self.toolOptionsEditor = ToolOptionsEditor(
            tool, 
            undoStack,
            settings)

        # All 
        layout = QtGui.QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(self.toolOptionsEditor)
        self.setLayout(layout)