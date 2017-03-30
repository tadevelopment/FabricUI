#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine import Core
from FabricEngine.Canvas.Utils import *
from FabricEngine.FabricUI import Commands
from FabricEngine.Canvas.Commands.CommandRegistry import *

class ToolTableWidget(QtGui.QTableWidget):

    """ ToolTableWidget  
    """

    def __init__(self, parent, canvasWindow):
        """ Initializes the ToolTableWidget.
            
            Arguments:
            - parent: A reference to wiget.
            - canvasWindow: A reference the canvasWindow.
        """
        super(ToolTableWidget, self).__init__(parent)

        self.canvasWindow = canvasWindow

        client = self.canvasWindow.client
        self.toolManager = client.RT.types.WidgetsManager.create()
        self.toolManager = self.toolManager.getWidgetsManager('WidgetsManager')
        self.toolManagerVersion = self.toolManager.getVersion('UInt32').getSimpleType()

        # Two columns: [nane, class, type]
        self.setColumnCount(3)
        self.setHorizontalHeaderItem(0, QtGui.QTableWidgetItem('Name'))
        self.setHorizontalHeaderItem(1, QtGui.QTableWidgetItem('Visible'))
        self.setHorizontalHeaderItem(2, QtGui.QTableWidgetItem('Truc2'))

        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragEnabled(False)
        self.setTabKeyNavigation(False)
        self.setSortingEnabled(True)
        self.verticalHeader().setVisible(False)
        self.horizontalHeader().setStretchLastSection(True)

        # qss
        self.setObjectName('ToolTableWidget')

    def showEvent(self, event):
        self.onRefresh()
        super(ToolTableWidget, self).showEvent(event)

    def __getToolCount(self):
        toolList = self.toolManager.drawnWidgets
        return toolList.count_slow('Size').getSimpleType()

    def __getToolAtIndex(self, index):
        toolList = self.toolManager.drawnWidgets
        return toolList.get('BaseWidget', index)

    def onRefresh(self):
        version = self.toolManager.getVersion('UInt32').getSimpleType()

        if version != self.toolManagerVersion:
            self.clearContents()

            toolCount = self.__getToolCount()
            print "toolCount " + str(toolCount)

            for i in range(0, toolCount):
                tool = self.__getToolAtIndex(i)
                toolName = tool.getName('String').getSimpleType()
                isVisible = tool.isVisible('Boolean').getSimpleType()

                self.__createNewRow(toolName)
                print "toolName " + str(toolName)


        self.toolManagerVersion = version

    def __createNewRow(self, toolName):
        """ \internal.
            Create a new row: [name, class, type] items.
        """

        rowCount = self.rowCount() 
        self.insertRow(rowCount)
        
        # Name item
        item = QtGui.QTableWidgetItem(toolName) 
        #item.setToolTip(cmd.getHelp())
        item.setFlags(QtCore.Qt.NoItemFlags)
        self.setItem(rowCount, 0, item)
        
        # # Class item
        # item = QtGui.QTableWidgetItem(cmdType) 
        # item.setFlags(QtCore.Qt.NoItemFlags)
        # self.setItem(rowCount, 1, item)
        
        # # Type item
        # item = QtGui.QTableWidgetItem(implType) 
        # item.setFlags(QtCore.Qt.NoItemFlags)
        # self.setItem(rowCount, 2, item)

        # Hack to refresh the view correctly.
        # self.setVisible(False)
        # self.resizeColumnsToContents()
        # self.setVisible(True)
  
    def filterItems(self, query):
        """ \internal.
            Filters the items according the commands' names, classes or types.
            To filter by class, use '#' before the query.  
            To filter by type, use '@' before the query.  
        """
        searchByClass = False
        if len(query):
            searchByClass = query[0] == '#'

        searchByType = False
        if len(query):
            searchByType = query[0] == '@'
   
        regex = CreateSearchRegex(query) 

        for i in range(0, self.rowCount()):

            # Checks the action's name/shortcut matches.
            cmdName = self.item(i, 0).text()
            cmdType = self.item(i, 1).text()
            implType = self.item(i, 2).text()

            if  (   (searchByClass and not searchByType and regex.search(cmdType.lower()) ) or 
                    (not searchByClass and searchByType and regex.search(implType.lower()) ) or 
                    (not searchByClass and not searchByType and  regex.search(cmdName.lower()) ) ):

                isCommand = GetCommandRegistry().isCommandRegistered(cmdName)
                self.setRowHidden(i, False)
                 
            else:
                self.setRowHidden(i, True)
