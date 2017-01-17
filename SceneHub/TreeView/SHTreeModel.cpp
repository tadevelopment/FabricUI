/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include "SHTreeModel.h"

using namespace FabricUI;
using namespace SceneHub;
using namespace FabricCore;


SHTreeModel::SHTreeModel(Client client, RTVal sceneGraph, QObject *parent)
  : QAbstractItemModel(parent)
  , m_client(client)
  , m_showProperties(false)
  , m_showOperators(false)
  , m_sceneHierarchyChangedBlockCount(0)
{
  setReferenceColor(QColor(Qt::white));
  setPropertyColor(QColor(Qt::blue).lighter(165));
  setOperatorColor(QColor(Qt::red).lighter(165));

  QFont propagatedFont;
  propagatedFont.setItalic(true);
  m_propagatedFontVariant = QVariant(propagatedFont);

  QFont overrideFont;
  overrideFont.setBold(true);
  m_overrideFontVariant = QVariant(overrideFont);

  QFont overridePropagatedFont;
  overridePropagatedFont.setItalic(true);
  overridePropagatedFont.setBold(true);
  m_overridePropagatedFontVariant = QVariant(overridePropagatedFont);

  try 
  {
    m_treeViewDataRTVal = RTVal::Create(
      m_client,
      "SGTreeViewData",
      1, 
      &sceneGraph);

    // Try to optimize a bit: share RTVals when updating nodes
    // However, there RTVal don't have methods to update content so the benefit is limited fornow.
    m_getUpdatedChildDataArgs[0] = RTVal::ConstructUInt32(m_client, 0);//Size index
    m_getUpdatedChildDataArgs[1] = RTVal::ConstructData(m_client, NULL);//Data externalOwnerID
    m_getUpdatedChildDataArgs[2] = RTVal::ConstructBoolean(m_client, false);//io Boolean invalidate
    m_getUpdatedChildDataArgs[3] = RTVal::ConstructBoolean(m_client, false);//io Boolean isPropagated
    m_getUpdatedChildDataArgs[4] = RTVal::ConstructBoolean(m_client, false);//io Boolean isOverride
    m_getUpdatedChildDataArgs[5] = RTVal::ConstructBoolean(m_client, false);//io Boolean isReference
    m_getUpdatedChildDataArgs[6] = RTVal::ConstructBoolean(m_client, false);//io Boolean isGenerator

    m_updateArgs[0] = RTVal::ConstructBoolean(m_client, m_showProperties);
    m_updateArgs[1] = RTVal::ConstructBoolean(m_client, m_showOperators);
    m_updateArgs[2] = RTVal::Construct(m_client, "SGTreeViewObjectDataChanges", 0, 0);//io SGTreeViewObjectDataChanges changes
  }
  catch (Exception e)
  {
    printf("SHTreeModel::SHTreeModel: Error: %s\n", e.getDesc_cstr());
  }
}

SHTreeModel::~SHTreeModel() { 
  for(RootItemsVec::iterator it = m_rootItems.begin(); it != m_rootItems.end(); ++it) {
    SHTreeItem *rootItem = *it;
    delete rootItem;
  }
}

QModelIndex SHTreeModel::addRootItem(RTVal rootSGObject) {
  SHTreeItem *item = new SHTreeItem(this, 0 /* parentItem */, m_client);
  try 
  {
    RTVal args[2];
    args[0] = rootSGObject;//SGObject root
    args[1] = RTVal::ConstructData(m_client, item);//Data externalOwnerID

    RTVal sgTreeViewObjectData = 
      m_treeViewDataRTVal.callMethod("SGTreeViewObjectData", "getOrCreateRootData", 2, args);

    item->updateNeeded(sgTreeViewObjectData, false);
  }
  catch (Exception e)
  {
    printf("SHTreeModel::addRootItem: Error: %s\n", e.getDesc_cstr());
  }

  int row = m_rootItems.size();
  QModelIndex index = createIndex(row, 0, item);
  item->setIndex(index);
  m_rootItems.push_back(item);

  return index;
}

void SHTreeModel::setShowProperties(bool show) {
  if(show != m_showProperties) 
  {
    m_showProperties = show;
    m_updateArgs[0].setBoolean(m_showProperties);
    onSceneHierarchyChanged();
  }
}

void SHTreeModel::setShowOperators(bool show) {
  if(show != m_showOperators) 
  {
    m_showOperators = show;
    m_updateArgs[1].setBoolean(m_showOperators);
    onSceneHierarchyChanged();
  }
}

void SHTreeModel::setPropertyColor(QColor color) { 
  m_propertyColorVariant = QVariant(color); 
}
    
void SHTreeModel::setReferenceColor(QColor color) { 
  m_referenceColorVariant = QVariant(color); 
}

void SHTreeModel::setOperatorColor(QColor color) { 
  m_operatorColorVariant = QVariant(color); 
}

std::vector<QModelIndex> SHTreeModel::getIndicesFromSGObject(RTVal sgObject) {
  std::vector<QModelIndex> indices;
  try 
  {
    RTVal dataArray = m_treeViewDataRTVal.callMethod("Data[]", "getExternalOwnerIDs", 1, &sgObject);

    unsigned int count = dataArray.getArraySize();
    for(unsigned int i = 0; i < count; ++i) 
    {
      SHTreeItem* item = (SHTreeItem*)dataArray.getArrayElement(i).getData();
      if(item) indices.push_back(item->getIndex());
    }
  }
  catch (Exception e)
  {
    printf("SHTreeModel::getIndicesFromSGObject: Error: %s\n", e.getDesc_cstr());
  }
  return indices;
}

QVariant SHTreeModel::data(const QModelIndex &index, int role) const {
  if(!index.isValid()) return QVariant();

  SHTreeItem *item = static_cast<SHTreeItem *>(index.internalPointer());
  
  if(role == Qt::DisplayRole)
    return item->desc();

  else if(role == Qt::ForegroundRole) 
  {
    if(item->isGenerator())
      return m_operatorColorVariant;
    if(item->isReference())
      return m_referenceColorVariant;
    return m_propertyColorVariant;
  } 

  else if(role == Qt::FontRole && item->isPropagated()) 
  {
    if(item->isPropagated() && item->isOverride())
      return m_overridePropagatedFontVariant;
    if(item->isPropagated())
      return m_propagatedFontVariant;
    if(item->isOverride())
      return m_overrideFontVariant;
  }
  return QVariant();
}

Qt::ItemFlags SHTreeModel::flags(const QModelIndex &index) const {
  if(!index.isValid()) return 0;
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex SHTreeModel::index(int row, int col, const QModelIndex &parentIndex) const {
  if(!hasIndex(row, col, parentIndex))
    return QModelIndex();

  SHTreeItem *item;
  if(parentIndex.isValid())
  {
    SHTreeItem *parentItem = static_cast<SHTreeItem *>(parentIndex.internalPointer());
    item = parentItem->childItem(row);
  }
  else item = m_rootItems[row];

  return createIndex(row, col, item);
}

QModelIndex SHTreeModel::parent(const QModelIndex &childIndex) const {
  if(!childIndex.isValid()) return QModelIndex();

  SHTreeItem *childItem = static_cast<SHTreeItem *>(childIndex.internalPointer());
  SHTreeItem *parentItem = childItem->parentItem();
  if(!parentItem)
    return QModelIndex();

  int row;
  if(SHTreeItem *grandParentItem = parentItem->parentItem())
    row = grandParentItem->childRow(parentItem);
  else
  {
    RootItemsVec::const_iterator it = std::find(
      m_rootItems.begin(),
      m_rootItems.end(),
      parentItem);
    assert(it != m_rootItems.end());
    row = it - m_rootItems.begin();
  }

  return createIndex(row, 0, parentItem);
}

int SHTreeModel::rowCount(const QModelIndex &index) const {
  int result;
  if(index.isValid())
  {
    SHTreeItem *item = static_cast<SHTreeItem *>(index.internalPointer());
    result = item->childItemCount();
  }
  else result = m_rootItems.size();
  return result;
}
 
int SHTreeModel::columnCount(const QModelIndex &index) const { 
  return 1; 
}

void SHTreeModel::onSceneHierarchyChanged() {
  SceneHierarchyChangedBlocker blocker(this);
  for(RootItemsVec::iterator it = m_rootItems.begin(); it != m_rootItems.end(); ++it)
  {
    SHTreeItem *rootItem = *it;
    rootItem->updateNeeded();
    rootItem->updateChildItemsIfNeeded();
  }
}

void SHTreeModel::emitSceneHierarchyChanged() {
  if(m_sceneHierarchyChangedBlockCount > 0)
    m_sceneHierarchyChangedPending = true;
  else
    emit sceneHierarchyChanged();
}

void SHTreeModel::emitSceneChanged() { 
  emit sceneChanged(); 
}
