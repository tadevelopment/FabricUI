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

SHVEEditorOwner::SHVEEditorOwner(DFG::DFGWidget * dfgWidget, SceneHub::SHTreeView * treeView)
  : DFG::DFGVEEditorOwner(dfgWidget)
  , m_treeView(treeView)
{
}

SHVEEditorOwner::~SHVEEditorOwner()
{
}

void SHVEEditorOwner::initConnections()
{
  DFG::DFGVEEditorOwner::initConnections();

  connect(
    m_treeView,
    SIGNAL(itemSelected( FabricUI::SceneHub::SHTreeItem * )),
    this,
    SLOT(onSceneItemSelected( FabricUI::SceneHub::SHTreeItem * ))
    );
}

void SHVEEditorOwner::onSceneItemSelected( FabricUI::SceneHub::SHTreeItem *item )
{
  m_valueEditor->clear();

  if(m_modelRoot)
    delete m_modelRoot;

  FabricCore::RTVal sgObject = item->getSGObject();
  m_modelRoot = new SGObjectModelItem(getDFGController()->getClient(), sgObject);
  emit replaceModelRoot( m_modelRoot );
}
