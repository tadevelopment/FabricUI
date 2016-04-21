//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "SHVEEditorOwner.h"

using namespace FabricUI;
using namespace SceneHub;
using namespace ModelItems;


SHVEEditorOwner::SHVEEditorOwner(DFG::DFGWidget * dfgWidget, SceneHub::SHBaseTreeView* baseTreeView)
  : SHBaseVEEditorOwner(dfgWidget)
  , m_baseTreeView(baseTreeView) {
}

SHVEEditorOwner::~SHVEEditorOwner() {}

void SHVEEditorOwner::initConnections() {
  DFG::DFGVEEditorOwner::initConnections();
  connect(
    m_baseTreeView,
    SIGNAL(itemSelected( FabricUI::SceneHub::SHTreeItem * )),
    this,
    SLOT(onSceneItemSelected( FabricUI::SceneHub::SHTreeItem * ))
    );

  connect(
    m_baseTreeView,
    SIGNAL(itemDoubleClicked( FabricUI::SceneHub::SHTreeItem * )),
    this,
    SLOT(onSceneItemSelected( FabricUI::SceneHub::SHTreeItem * ))
    );
}

void SHVEEditorOwner::onUpdateSGObject(SHDFGBinding *shDFGBinding) {
  updateSGObject(shDFGBinding->getSgObject());
}

void SHVEEditorOwner::onStructureChanged() {
  DFG::DFGVEEditorOwner::onStructureChanged();

  // refresh!
  SGObjectModelItem * objectItem = castToSGModelItem( m_modelRoot );
  if(objectItem) 
  {
    objectItem->onStructureChanged();
    emit replaceModelRoot( m_modelRoot );
  }
}

void SHVEEditorOwner::onSceneItemSelected( FabricUI::SceneHub::SHTreeItem * item ) {
  FabricCore::RTVal sgObject = item->getSGObject();
  if( sgObject.isValid() )
    updateSGObject( sgObject );
  else {
    FabricCore::RTVal sgObjectProperty = item->getSGObjectProperty();
    if( sgObjectProperty.isValid() )
      updateSGObjectProperty( sgObjectProperty );
  }
}

void SHVEEditorOwner::onSceneChanged() {
  SGObjectModelItem * objectItem = castToSGModelItem( m_modelRoot );
  if( objectItem ) {
    //Important: take a value copy since passed by ref and sgObject might be deleted
    FabricCore::RTVal sgObject = objectItem->getSGObject();
    updateSGObject( sgObject );
  } else if( m_objectPropertyItem ) {
      //Important: take a value copy since passed by ref and sgObject might be deleted
    FabricCore::RTVal sgObjectProperty = m_objectPropertyItem->getSGObjectProperty();
    updateSGObjectProperty( sgObjectProperty );
  }
}

void SHVEEditorOwner::onSidePanelInspectRequested() {
  FTL::CStrRef execPath = getDFGController()->getExecPath();
  if(execPath.empty())
    emit canvasSidePanelInspectRequested();
}
