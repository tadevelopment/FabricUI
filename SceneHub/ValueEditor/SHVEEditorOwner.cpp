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
  onNewSGObjectSet( item->getSGObject() );
}

void SHVEEditorOwner::onNewSGObjectSet( FabricCore::RTVal sgObject )
{
  m_valueEditor->clear();

  if(m_modelRoot)
    delete m_modelRoot;

  SGObjectModelItem * objectItem = new SGObjectModelItem( m_cmdViewWidget, getDFGController()->getClient(), sgObject );
  QObject::connect(objectItem, SIGNAL( propertyItemInserted( BaseModelItem * ) ), this, SLOT( onSGObjectPropertyItemInserted( BaseModelItem * ) ));

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
