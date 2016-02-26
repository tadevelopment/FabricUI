//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "VEEditorOwner.h"

#include <FabricUI/ModelItems/BindingModelItem.h>
#include <FabricUI/ModelItems/GetModelItem.h>
#include <FabricUI/ModelItems/InstModelItem.h>
#include <FabricUI/ModelItems/SetModelItem.h>
#include <FabricUI/ModelItems/VarModelItem.h>
#include <FabricUI/ValueEditor/ItemMetadata.h>
#include <FabricUI/ValueEditor/BaseViewItem.h>
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>

using namespace FabricUI;
using namespace ValueEditor;
using namespace ModelItems;

VEEditorOwner::VEEditorOwner( ValueEditorBridgeOwner *owner )
  : m_owner( owner )
  , m_valueEditor( NULL )
  , m_modelRoot( NULL )
{
  m_valueEditor = new VETreeWidget();
}

VEEditorOwner::~VEEditorOwner()
{
  delete m_modelRoot;
}

QWidget* VEEditorOwner::getWidget() const
{
  return m_valueEditor;
}

void VEEditorOwner::initConnections()
{
  connect(
    this, SIGNAL( modelItemInserted( BaseModelItem*, int, const char* ) ),
    m_valueEditor, SLOT( onModelItemChildInserted( BaseModelItem*, int, const char* ) )
    );
  connect(
    this, SIGNAL( modelItemTypeChange( BaseModelItem*, const char* ) ),
    m_valueEditor, SLOT( onModelItemTypeChanged( BaseModelItem*, const char* ) )
    );
  connect(
    this, SIGNAL( modelItemRemoved( BaseModelItem* ) ),
    m_valueEditor, SLOT( onModelItemRemoved( BaseModelItem* ) )
    );
  connect(
    this, SIGNAL( modelItemRenamed( BaseModelItem* ) ),
    m_valueEditor, SLOT( onModelItemRenamed( BaseModelItem* ) )
    );
  connect(
    this, SIGNAL( modelItemChildrenReordered( BaseModelItem*, const QList<int>& ) ),
    m_valueEditor, SLOT( onModelItemChildrenReordered( BaseModelItem*, const QList<int>& ) )
    );
  connect(
    this, SIGNAL( replaceModelRoot( BaseModelItem* ) ),
    m_valueEditor, SLOT( onSetModelItem( BaseModelItem* ) )
    );
}
