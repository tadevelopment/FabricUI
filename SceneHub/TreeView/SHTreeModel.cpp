/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <iostream>
#include "SHTreeModel.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SHTreeModel::SHTreeModel( SHGLScene *shGLScene, QObject *parent )
  : QAbstractItemModel( parent )
  , m_client( shGLScene->getClient() )
  , m_sceneHierarchyChangedBlockCount( 0 )
{
  try {
    FabricCore::RTVal sg = shGLScene->getSG();
    m_treeViewDataRTVal =
      FabricCore::RTVal::Create(
      m_client,
      "SGTreeViewData",
      1, &sg
      );

    // Try to optimize a bit: share RTVals when updating nodes
    // However, there RTVal don't have methods to update content so the benefit is limited for now.
    m_getUpdatedChildDataArgs[0] = FabricCore::RTVal::ConstructUInt32( m_client, 0 );//Size index
    m_getUpdatedChildDataArgs[1] = FabricCore::RTVal::ConstructData( m_client, NULL );//Data externalOwnerID
    m_getUpdatedChildDataArgs[2] = FabricCore::RTVal::ConstructBoolean( m_client, false );//io Boolean invalidate

    m_updateArgs[0] = FabricCore::RTVal::ConstructBoolean( m_client, false );//Boolean includeProperties
    m_updateArgs[1] = FabricCore::RTVal::Construct( m_client, "SGTreeViewObjectDataChanges", 0, 0 );//io SGTreeViewObjectDataChanges changes
  }
  catch ( FabricCore::Exception e )
  {
    printf("SHTreeModel::SHTreeModel: Error: %s\n", e.getDesc_cstr());
  }
}

SHTreeModel::SHTreeModel( FabricCore::Client client, FabricCore::RTVal sceneGraph, QObject *parent )
  : QAbstractItemModel( parent )
  , m_client( client )
  , m_sceneHierarchyChangedBlockCount( 0 )
{
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

    m_updateArgs[0] = FabricCore::RTVal::ConstructBoolean( m_client, false );//Boolean includeProperties
    m_updateArgs[1] = FabricCore::RTVal::Construct( m_client, "SGTreeViewObjectDataChanges", 0, 0 );//io SGTreeViewObjectDataChanges changes
  }
  catch ( FabricCore::Exception e )
  {
    printf("SHTreeModel::SHTreeModel: Error: %s\n", e.getDesc_cstr());
  }
}

SHTreeModel::~SHTreeModel() {
  for ( RootItemsVec::iterator it = m_rootItems.begin(); it != m_rootItems.end(); ++it )
  {
    SHTreeItem *rootItem = *it;
    delete rootItem;
  }
}

QModelIndex SHTreeModel::addRootItem(FabricCore::RTVal rootSGObject) {
  SHTreeItem *item = 0;
  try {
    item = new SHTreeItem( this, 0 /* parentItem */, m_client );
    FabricCore::RTVal args[2];
    args[0] = rootSGObject;//SGObject root
    args[1] = FabricCore::RTVal::ConstructData( m_client, item );//Data externalOwnerID

    FabricCore::RTVal sgTreeViewObjectData = 
      m_treeViewDataRTVal.callMethod( "SGTreeViewObjectData", "getOrCreateRootData", 2, args );

    item->updateNeeded( sgTreeViewObjectData, false );
  }
  catch ( FabricCore::Exception e )
  {
    printf("SHTreeModel::addRootItem: Error: %s\n", e.getDesc_cstr());
  }
  int row = m_rootItems.size();
  QModelIndex index = createIndex( row, 0, item );
  item->setIndex( index );

  m_rootItems.push_back( item );
  return index;
}

QModelIndex SHTreeModel::addRootItemsFromScene(SHGLScene *shGLScene) {
  QModelIndex sceneRootIndex = addRootItem(shGLScene->getSceneRoot());
  addRootItem(shGLScene->getAssetLibraryRoot());
  addRootItem(shGLScene->getMaterialLibraryRoot());
  addRootItem(shGLScene->getImageLibraryRoot());
  return sceneRootIndex;
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

QVariant SHTreeModel::data(const QModelIndex &index, int role) const {
  if ( !index.isValid() || role != Qt::DisplayRole )
    return QVariant();
  SHTreeItem *item = static_cast<SHTreeItem *>( index.internalPointer() );
  return item->desc();
}

Qt::ItemFlags SHTreeModel::flags( const QModelIndex &index ) const {
  if ( !index.isValid() )
    return 0;
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex SHTreeModel::index(int row, int col, const QModelIndex &parentIndex ) const {
  if ( !hasIndex( row, col, parentIndex ) )
    return QModelIndex();

  SHTreeItem *item;
  if ( parentIndex.isValid() ) 
  {
    SHTreeItem *parentItem = static_cast<SHTreeItem *>( parentIndex.internalPointer() );
    item = parentItem->childItem( row );
  }
  else item = m_rootItems[row];

  return createIndex( row, col, item );
}

QModelIndex SHTreeModel::parent( const QModelIndex &childIndex ) const {
  if ( !childIndex.isValid() )
    return QModelIndex();

  SHTreeItem *childItem =
    static_cast<SHTreeItem *>( childIndex.internalPointer() );
  SHTreeItem *parentItem = childItem->parentItem();
  if ( !parentItem )
    return QModelIndex();

  int row;
  if ( SHTreeItem *grandParentItem = parentItem->parentItem() )
    row = grandParentItem->childRow( parentItem );
  else
  {
    RootItemsVec::const_iterator it =
      std::find(
        m_rootItems.begin(),
        m_rootItems.end(),
        parentItem
        );
    assert( it != m_rootItems.end() );
    row = it - m_rootItems.begin();
  }
  return createIndex( row, 0, parentItem );
}

int SHTreeModel::rowCount( const QModelIndex &index ) const {
  int result;
  if ( index.isValid() )
  {
    SHTreeItem *item = static_cast<SHTreeItem *>( index.internalPointer() );
    result = item->childItemCount();
  }
  else result = m_rootItems.size();
  return result;
}

void SHTreeModel::onSceneHierarchyChanged() {
  SceneHierarchyChangedBlocker blocker( this );
  for ( RootItemsVec::iterator it = m_rootItems.begin(); it != m_rootItems.end(); ++it )
  {
    SHTreeItem *rootItem = *it;
    rootItem->updateNeeded();
    rootItem->updateChildItemsIfNeeded();
  }
}

