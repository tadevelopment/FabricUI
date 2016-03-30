//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "SHVEEditorOwner.h"
#include "SGObjectModelItem.h"
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>
#include <FabricUI/DFG/DFGController.h>

using namespace FabricUI;
using namespace SceneHub;
using namespace ModelItems;

SHVEEditorOwner::SHVEEditorOwner(DFG::DFGWidget * dfgWidget, SceneHub::SHTreeViewWidget * treeViewWidget, SHCmdViewWidget * cmdViewWidget)
  : DFG::DFGVEEditorOwner(dfgWidget)
  , m_treeViewWidget(treeViewWidget)
  , m_cmdViewWidget(cmdViewWidget)
{
}

SHVEEditorOwner::~SHVEEditorOwner()
{
}

void SHVEEditorOwner::initConnections()
{
  DFG::DFGVEEditorOwner::initConnections();

  connect(
    m_treeViewWidget->getTreeView(),
    SIGNAL(itemSelected( FabricUI::SceneHub::SHTreeItem * )),
    this,
    SLOT(onSceneItemSelected( FabricUI::SceneHub::SHTreeItem * ))
    );
  connect(
    m_treeViewWidget->getTreeView(),
    SIGNAL(itemDoubleClicked( FabricUI::SceneHub::SHTreeItem * )),
    this,
    SLOT(onSceneItemSelected( FabricUI::SceneHub::SHTreeItem * ))
    );
}

void SHVEEditorOwner::onStructureChanged()
{
  DFG::DFGVEEditorOwner::onStructureChanged();

  // refresh!
  SGObjectModelItem * objectItem = dynamic_cast< SGObjectModelItem * >( m_modelRoot );
  if(objectItem) 
  {
    objectItem->onStructureChanged();
    emit replaceModelRoot( m_modelRoot );
  }
}

void SHVEEditorOwner::onSceneItemSelected( FabricUI::SceneHub::SHTreeItem * item )
{
  FabricCore::RTVal sgObject = item->getSGObject();
  if( sgObject.isValid() )
    updateSGObject( sgObject );
}

void SHVEEditorOwner::onSceneChanged() {
  SGObjectModelItem * objectItem = dynamic_cast< SGObjectModelItem * >( m_modelRoot );
  if( objectItem ) {
    //Important: take a value copy since passed by ref and sgObject might be deleted
    FabricCore::RTVal sgObject = objectItem->getSGObject();
    updateSGObject( sgObject );
  }
}

void SHVEEditorOwner::updateSGObject( const FabricCore::RTVal& sgObject )
{
  bool isValid = true;
  bool structureChanged = true;

  SGObjectModelItem * objectItem = dynamic_cast< SGObjectModelItem * >( m_modelRoot );
  if( objectItem )
    objectItem->updateFromScene( sgObject, isValid, structureChanged );

  if( !structureChanged )
    return;

  // Currently we don't support incremental structure changes; we just rebuild all if it was the case

  m_valueEditor->clear();
  if( m_modelRoot )
    delete m_modelRoot;
  objectItem = 0;

  if( sgObject.isValid() && isValid ) {
    objectItem = new SGObjectModelItem( m_cmdViewWidget, getDFGController()->getClient(), sgObject );
    QObject::connect( objectItem, SIGNAL( propertyItemInserted( BaseModelItem * ) ), this, SLOT( onSGObjectPropertyItemInserted( BaseModelItem * ) ) );
  }

  m_modelRoot = objectItem;
  emit replaceModelRoot( m_modelRoot );
}

void SHVEEditorOwner::onSGObjectPropertyItemInserted( BaseModelItem * item )
{
  emit modelItemInserted( m_modelRoot, 0, item->getName().c_str() );
}

void SHVEEditorOwner::onSidePanelInspectRequested()
{
  FTL::CStrRef execPath = getDFGController()->getExecPath();
  if(execPath.empty())
    emit canvasSidePanelInspectRequested();
}
