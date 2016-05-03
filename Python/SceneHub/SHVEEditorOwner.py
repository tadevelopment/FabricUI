
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
 
class SHVEEditorOwner(SceneHub.SHBaseVEEditorOwner):

  	"""SHVEEditorOwner

    SHVEEditorOwner specializes SceneHub.SHBaseVEEditorOwner.
    It gives access to the base signals/slots so it can be specialized if needed.

    Arguments:
        client (FabricEngine.Core.Client): A reference to the FabricCore.Client.
        dfgWidget (DFG.DFGWidget): A reference to the DFGWidget.
        shStates (SceneHub.SHStates): A reference to the SHStates.
    """

	canvasSidePanelInspectRequested = QtCore.Signal()

	def __init__(self, dfgWidget, shStates):
		self.shStates = shStates
		super(SHVEEditorOwner, self).__init__(dfgWidget)
 
	def onStructureChanged(self):
		""" Updates the valueEditor when the current rootItem properties
		    are changed by another modules (from the canvas graph, from manipulators...).
        """

		super(SHVEEditorOwner, self).onStructureChanged()
		objectItem = self.castToSGModelItem(self.m_modelRoot)
		if objectItem is not None: 
			objectItem.onStructureChanged()
			self.replaceModelRoot.emit(self.m_modelRoot)

	def onInspectChanged(self):
	 	""" Override, uses to display the selection properties in the valueEditor.
        """

		if self.shStates.isInspectingSGObject() or self.shStates.isInspectingSGCanvasOperator():
			sgObject = self.shStates.getInspectedSGObject()
			self.updateSGObject(sgObject)

		elif self.shStates.isInspectingSGObjectProperty():
			sgObjectProperty = self.shStates.getInspectedSGObjectProperty()
			self.updateSGObjectProperty(sgObjectProperty)
		
	def onSceneChanged(self):
		""" Updates the valuesEdditor Tree when the root_item changed.
        """

		objectItem = self.castToSGModelItem(self.m_modelRoot)
		if objectItem is not None: 
			#Important: take a value copy since passed by ref and sgObject might be deleted
			sgObject = objectItem.getSGObject()
			self.updateSGObject(sgObject)
	 	elif self.getSGObjectPropertyModelItem() is not None:
			#Important: take a value copy since passed by ref and sgObject might be deleted
			sgObjectProperty = self.getSGObjectPropertyModelItem().getSGObjectProperty()
			self.updateSGObjectProperty(sgObjectProperty)	

	def onSidePanelInspectRequested(self):
		execPath = self.getDFGController().getExecPath()
		if not execPath:
			self.canvasSidePanelInspectRequested.emit()
	
