/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <iostream>
#include "SHTreeModel.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SHTreeModel::SHTreeModel(int, FabricCore::Client client, QObject *parent )
  : QAbstractItemModel( parent )
  , m_client( client )
  , m_showProperties( false )
  , m_showOperators( false )
  , m_sceneHierarchyChangedBlockCount( 0 )
{
  initStyle();
}

SHTreeModel::SHTreeModel( FabricCore::Client client, FabricCore::RTVal sceneGraph, QObject *parent )
  : QAbstractItemModel( parent )
  , m_client( client )
  , m_showProperties( false )
  , m_showOperators( false )
  , m_sceneHierarchyChangedBlockCount( 0 )
{
  initStyle();
  try {
    m_treeViewDataRTVal =
      FabricCore::RTVal::Create(
      m_client,
      "SGTreeViewData",
      1, &sceneGraph
      );

    // Try to optimize a bit: share RTVals when updating nodes
    // However, there RTVal don't have methods to update content so the benefit is limited for now.
    m_getUpdatedChildDataArgs[0] = FabricCore::RTVal::ConstructUInt32( m_client, 0 );//Size index
    m_getUpdatedChildDataArgs[1] = FabricCore::RTVal::ConstructData( m_client, NULL );//Data externalOwnerID
    m_getUpdatedChildDataArgs[2] = FabricCore::RTVal::ConstructBoolean( m_client, false );//io Boolean invalidate
    m_getUpdatedChildDataArgs[3] = FabricCore::RTVal::ConstructBoolean( m_client, false );//io Boolean isPropagated
    m_getUpdatedChildDataArgs[4] = FabricCore::RTVal::ConstructBoolean( m_client, false );//io Boolean isReference
    m_getUpdatedChildDataArgs[5] = FabricCore::RTVal::ConstructBoolean( m_client, false );//io Boolean isGenerator

    m_updateArgs[0] = FabricCore::RTVal::ConstructBoolean( m_client, m_showProperties );
    m_updateArgs[1] = FabricCore::RTVal::ConstructBoolean( m_client, m_showOperators );
    m_updateArgs[2] = FabricCore::RTVal::Construct( m_client, "SGTreeViewObjectDataChanges", 0, 0 );//io SGTreeViewObjectDataChanges changes
  }
  catch ( FabricCore::Exception e )
  {
    printf("SHTreeModel::SHTreeModel: Error: %s\n", e.getDesc_cstr());
  }
}

void SHTreeModel::initStyle() {
  setReferenceColor( QColor( Qt::white ) );
  setPropertyColor( QColor( Qt::blue ).lighter( 165 ) );
  setOperatorColor( QColor( Qt::red ).lighter( 165 ) );
  QFont overrideFont;
  overrideFont.setItalic( true );
  m_overrideFontVariant = QVariant( overrideFont );
}

void SHTreeModel::setShowProperties( bool show ) {
  if( show != m_showProperties ) {
    m_showProperties = show;
    m_updateArgs[0].setBoolean( m_showProperties );
    onSceneHierarchyChanged();
  }
}

void SHTreeModel::setShowOperators( bool show ) {
  if( show != m_showOperators ) {
    m_showOperators = show;
    m_updateArgs[1].setBoolean( m_showOperators );
    onSceneHierarchyChanged();
  }
}

std::vector< QModelIndex > SHTreeModel::getIndicesFromSGObject( FabricCore::RTVal sgObject ) {
  std::vector< QModelIndex > indices;
  try {
    FabricCore::RTVal dataArray = m_treeViewDataRTVal.callMethod("Data[]", "getExternalOwnerIDs", 1, &sgObject);

    unsigned int count = dataArray.getArraySize();
    for( unsigned int i = 0; i < count; ++i ) {
      SHTreeItem* item = (SHTreeItem*)dataArray.getArrayElement( i ).getData();
      if( item )
        indices.push_back( item->getIndex() );
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf("SHTreeModel::getIndicesFromSGObject: Error: %s\n", e.getDesc_cstr());
  }
  return indices;
}
