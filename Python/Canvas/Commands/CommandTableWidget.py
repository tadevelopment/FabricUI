#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

from PySide import QtGui
from FabricEngine.Canvas.Utils import *
from FabricEngine.FabricUI import Commands
from FabricEngine.Canvas.Commands.CommandRegistry import *

class CommandTableWidget(QtGui.QTableWidget):

    """ CommandTableWidget  
    """

    def __init__(self, parent, canvasWindow):
        """ Initializes the CommandTableWidget.
            
            Arguments:
            - parent: A reference to wiget.
            - canvasWindow: A reference the canvasWindow.
        """
        super(CommandTableWidget, self).__init__(parent)

        self.canvasWindow = canvasWindow

        # Notify when an command is registered.
        GetCommandRegistry().commandRegisteredCallback.connect(self.__onCommandRegistered)
 
        # Two columns: [nane, class, type]
        self.setColumnCount(3)
        self.setHorizontalHeaderItem(0, QtGui.QTableWidgetItem('Name'))
        self.setHorizontalHeaderItem(1, QtGui.QTableWidgetItem('Class'))
        self.setHorizontalHeaderItem(2, QtGui.QTableWidgetItem('Type'))

        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragEnabled(False)
        self.setTabKeyNavigation(False)
        self.setSortingEnabled(True)
        self.verticalHeader().setVisible(False)
        self.horizontalHeader().setStretchLastSection(True)

        # qss
        self.setObjectName('CommandTableWidget')

    def __createNewRow(self, cmdName, cmd, cmdType, implType):
        """ \internal.
            Create a new row: [name, class, type] items.
        """

        rowCount = self.rowCount() 
        self.insertRow(rowCount)
        
        # Name item
        item = QtGui.QTableWidgetItem(cmdName) 
        item.setToolTip(cmd.getHelp())
        item.setFlags(QtCore.Qt.NoItemFlags)
        self.setItem(rowCount, 0, item)
        
        # Class item
        item = QtGui.QTableWidgetItem(cmdType) 
        item.setFlags(QtCore.Qt.NoItemFlags)
        self.setItem(rowCount, 1, item)
        
        # Type item
        item = QtGui.QTableWidgetItem(implType) 
        item.setFlags(QtCore.Qt.NoItemFlags)
        self.setItem(rowCount, 2, item)

        # Hack to refresh the view correctly.
        self.setVisible(False)
        self.resizeColumnsToContents()
        self.setVisible(True)

    def __onCommandRegistered(self, cmdName, cmdType, implType):
        """ \internal.
            Called when an command has been registered in CommandRegistry.
            Create an item associated to the command. 
        """
    
        # Must construct the command to get the tooltip
        cmd = GetCommandRegistry().createCommand(cmdName)
        self.__createNewRow(cmdName, cmd, cmdType, implType)
  
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
