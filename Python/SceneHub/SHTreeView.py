from FabricEngine import Core, FabricUI, Util, CAPI
from PySide import QtCore, QtGui
from FabricEngine.FabricUI import *
from FabricEngine.Util import *
from FabricEngine.CAPI import *
from FabricEngine.SceneHub.SHAssetsMenu import SHAssetsMenu
from FabricEngine.SceneHub.SHLightsMenu import SHLightsMenu
from FabricEngine.SceneHub.SHContextualMenu import SHContextualMenu


class SHTreeView(SceneHub.SHBaseTreeView):

    """SHTreeView

    SHTreeView specializes SceneHub.SHBaseTreeView.    
    It gives access to the base signals/slots so it can be specialized if needed.

    Arguments:
        client (FabricEngine.Core.Client): A reference to the FabricCore.Client.
        shStates (SceneHub.SHStates): A reference to the SHStates.
        shGLScene (SceneHub.SHGLScene): A reference to a SHGLScene.
    """

    selectionCleared = QtCore.Signal()
    itemSelected = QtCore.Signal(SceneHub.SHTreeItem)
    itemDeselected = QtCore.Signal(SceneHub.SHTreeItem)
    itemDoubleClicked = QtCore.Signal(SceneHub.SHTreeItem)


    def __init__(self, client, shStates, shGLScene):
        super(SHTreeView, self).__init__(client)
        self.client = client
        self.shGLScene = shGLScene
        self.shStates = shStates
        self.setHeaderHidden(True)
        self.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.customContextMenuRequested.connect(self.onCustomContextMenu)
  
    def onCustomContextMenu(self, point):
        """ Implementation of :`QTreeView`,
            Displays the contextual Menu.
        """

        index = self.indexAt(point);
        item = SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index)

        # SHContextualMenu being based on the C++ class SHBaseContexctualMenu
        # We need to explicitly construct a RTVal for the parameter sgObject
        sgObject = self.client.RT.types.Object()
        if item: 
            sgObject = item.getSGObject()   

        if (item is not None and sgObject is not None) or item is None:
            menu = SHContextualMenu(self.shGLScene, self.shStates, sgObject, self, self)
            menu.exec_(self.mapToGlobal(point))
    
  
    def selectionChanged(self, selected, deselected):
        """ Selects/Unselects treeView items.
            Implementation of : `SceneHub::SHBaseTreeView`
        """

        # clear selection (make sure 3D view is synchronized) if all elements are newly added
        clear = len(self.selectionModel().selectedIndexes()) == len(selected.indexes())
        super(SHTreeView, self).selectionChanged(selected, deselected)

        for index in deselected.indexes(): 
            self.itemDeselected.emit(SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index))
        if clear:
            self.selectionCleared.emit()
        for index in selected.indexes(): 
            self.itemSelected.emit(SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index))
 
    def mousePressEvent(self, event):
        """ Override, creates also a drag event to set assets/textures 
            from the treeView to the 3DView (Viewports).
        """

        if event.button() == QtCore.Qt.LeftButton:
            urlsList = []
            for index in self.selectedIndexes():
                item = SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index)
                sgObj = item.getSGObject()
                if sgObj is not None:
                    url = self.shGLScene.getTreeItemPath(item.getSGObject())
                    if url != "": 
                        urlsList.append(QtCore.QUrl(url))

            if len(urlsList) > 0:
                mimeData = QtCore.QMimeData()
                mimeData.setUrls(urlsList)
                # Create drag
                drag = QtGui.QDrag(self)
                drag.setMimeData(mimeData)
                drag.exec_(QtCore.Qt.CopyAction)

        super(SHTreeView, self).mousePressEvent(event)

    def mouseDoubleClickEvent(self, event):
        """ Override, uses the display the selection properties in the valueEditor.
        """

        for index in self.selectedIndexes():
            item = SceneHub.SHBaseTreeView.GetTreeItemAtIndex(index)
            if item is not None:
                self.itemDoubleClicked.emit(item)   
