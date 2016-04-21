
from PySide import QtCore, QtGui
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import *
 
class SHVEEditorOwner(SceneHub.SHBaseVEEditorOwner):

	canvasSidePanelInspectRequested = QtCore.Signal()

	def __init__(self, dfgWidget, shStates):
		self.shStates = shStates
		super(SHVEEditorOwner, self).__init__(dfgWidget)
 
	def onUpdateSGObject(self, shDFGBinding):
		self.updateSGObject(shDFGBinding.getSgObject())
	
	def onStructureChanged(self):
		super(SHVEEditorOwner, self).onStructureChanged()
		objectItem = self.castToSGModelItem( self.m_modelRoot )
		if objectItem is not None: 
			objectItem.onStructureChanged()
			self.replaceModelRoot.emit( self.m_modelRoot )

	def onInspectChanged(self):
		if self.shStates.isInspectingSGObject() or self.shStates.isInspectingSGCanvasOperator():
			sgObject = self.shStates.getInspectedSGObject()
			self.updateSGObject( sgObject )

		elif self.shStates.isInspectingSGObjectProperty():
			sgObjectProperty = self.shStates.getInspectedSGObjectProperty()
			self.updateSGObjectProperty( sgObjectProperty )
		
	def onSceneChanged(self):
		objectItem = self.castToSGModelItem( self.m_modelRoot )
		if objectItem is not None: 
			#Important: take a value copy since passed by ref and sgObject might be deleted
			sgObject = objectItem.getSGObject()
			self.updateSGObject( sgObject )
	 	elif self.getSGObjectPropertyModelItem() is not None:
			#Important: take a value copy since passed by ref and sgObject might be deleted
			sgObjectProperty = self.getSGObjectPropertyModelItem().getSGObjectProperty()
			self.updateSGObjectProperty( sgObjectProperty )	

	def onSidePanelInspectRequested(self):
		execPath = self.getDFGController().getExecPath()
		if not execPath:
			self.canvasSidePanelInspectRequested.emit()
	
