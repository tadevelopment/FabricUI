//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "SHVEEditorOwner.h"

using namespace FabricUI;
using namespace SceneHub;
using namespace ModelItems;


SHVEEditorOwner::SHVEEditorOwner(DFG::DFGWidget *dfgWidget, SHStates* shStates)
  : SHBaseVEEditorOwner(dfgWidget)
  , m_shStates(shStates) {
}

SHVEEditorOwner::~SHVEEditorOwner() {}
 
void SHVEEditorOwner::onStructureChanged() {
  DFG::DFGVEEditorOwner::onStructureChanged();

  // refresh!
  SGObjectModelItem * objectItem = castToSGModelItem(m_modelRoot);
  if(objectItem) 
  {
    objectItem->onStructureChanged();
    emit replaceModelRoot(m_modelRoot);
  }
}

void SHVEEditorOwner::onInspectChanged() {
  if(m_shStates->isInspectingSGObject() || m_shStates->isInspectingSGCanvasOperator())
  {
    FabricCore::RTVal sgObject = m_shStates->getInspectedSGObject();
    updateSGObject(sgObject);
  }

  else if(m_shStates->isInspectingSGObjectProperty())
  { 
    FabricCore::RTVal sgObjectProperty = m_shStates->getInspectedSGObjectProperty();
    updateSGObjectProperty(sgObjectProperty);
  }
}

void SHVEEditorOwner::onSceneChanged() {
  SGObjectModelItem *objectItem = castToSGModelItem(m_modelRoot);
  if(objectItem) 
  {
    //Important: take a value copy since passed by ref and sgObject might be deleted
    FabricCore::RTVal sgObject = objectItem->getSGObject();
    updateSGObject(sgObject);
  } 
  else if(m_objectPropertyItem) 
  {
    //Important: take a value copy since passed by ref and sgObject might be deleted
    FabricCore::RTVal sgObjectProperty = m_objectPropertyItem->getSGObjectProperty();
    updateSGObjectProperty(sgObjectProperty);
  }
}

void SHVEEditorOwner::onSidePanelInspectRequested() {
  FTL::CStrRef execPath = getDFGController()->getExecPath();
  if(execPath.empty())
    emit canvasSidePanelInspectRequested();
}
