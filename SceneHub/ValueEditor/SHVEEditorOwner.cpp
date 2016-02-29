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
  SGObjectModelItem * objectItem = new SGObjectModelItem(getDFGController()->getClient(), sgObject);
  QObject::connect(objectItem, SIGNAL( propertyItemInserted( BaseModelItem * ) ), this, SLOT( onSGObjectPropertyItemInserted( BaseModelItem * ) ));

  m_modelRoot = objectItem;
  emit replaceModelRoot( m_modelRoot );
}

void SHVEEditorOwner::onSGObjectPropertyItemInserted( BaseModelItem * item )
{
  emit modelItemInserted( m_modelRoot, 0, item->getName().c_str() );
}

