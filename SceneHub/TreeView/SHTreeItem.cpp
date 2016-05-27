/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHTreeItem.h"
#include "SHTreeModel.h"
 
using namespace FabricUI;
using namespace SceneHub;
using namespace FabricCore;

SHTreeItem::SHTreeItem(
  SHTreeModel *model, 
  SHTreeItem *parentItem, 
  Client client)
  : m_model(model)
  , m_parentItem(parentItem)
  , m_client(client)
  , m_needsUpdate(true)
  , m_hadInitialUpdate(false)
  , m_isPropagated(false)
  , m_isOverride(false)
  , m_isReference(false)
  , m_isGenerator(false)
{
  if(!parentItem)
    m_isReference = true;
}

QString SHTreeItem::desc() {
  if(m_treeViewObjectDataRTVal.isValid() && !m_treeViewObjectDataRTVal.isNullObject())
    return m_treeViewObjectDataRTVal.callMethod("String", "getName", 0, 0).getStringCString();
  else return m_name;
}

void SHTreeItem::setIndex(QModelIndex index) { 
  m_index = index; 
}

QModelIndex SHTreeItem::getIndex() { 
  return m_index; 
}

void SHTreeItem::setName(QString name) { 
  m_name = name; 
}

bool SHTreeItem::isObject() const { 
  return m_isReference; 
}

bool SHTreeItem::isReference() const { 
  return m_isReference; 
}

bool SHTreeItem::isPropagated() const { 
  return m_isPropagated; 
}

bool SHTreeItem::isOverride() const { 
  return m_isOverride; 
}

bool SHTreeItem::isGenerator() const { 
  return m_isGenerator; 
}

SHTreeItem *SHTreeItem::parentItem() { 
  return m_parentItem; 
}

SHTreeItem *SHTreeItem::childItem(int row) { 
  return getOrCreateChildItem(row); 
}

int SHTreeItem::childItemCount() {
  updateChildItemsIfNeeded();
  return m_childItems.size();
}

int SHTreeItem::childRow(SHTreeItem *childItem) {
  int row = 0;
  for(ChildItemVec::const_iterator it = m_childItems.begin(); it != m_childItems.end(); ++it, ++row) {
    if(childItem == it->m_child.get())
      return row;
  }
  assert(false);
  return 0;
}

RTVal SHTreeItem::getSGObject() {
  RTVal sgObjectRTVal;
  try 
  {
    RTVal validRTVal = RTVal::ConstructBoolean(m_client, false);
    if(m_treeViewObjectDataRTVal.isValid() && !m_treeViewObjectDataRTVal.isNullObject()) 
      sgObjectRTVal = m_treeViewObjectDataRTVal.callMethod("SGObject", "getObject", 1, &validRTVal);
    
    else if(m_parentItem && m_parentItem->m_treeViewObjectDataRTVal.isValid() && 
           !m_parentItem->m_treeViewObjectDataRTVal.isNullObject()) 
    {
      // Might be a generator, in which case we return the generator owned container
      RTVal args[2];
      args[0] = RTVal::ConstructUInt32(m_client, m_index.row());
      args[1] = validRTVal;
      sgObjectRTVal = m_parentItem->m_treeViewObjectDataRTVal.callMethod("SGObject", "getGeneratorOwnedObject", 2, args);
    }

    if(!validRTVal.getBoolean())
      sgObjectRTVal = RTVal();
  }
  catch(Exception e) 
  {
    printf("SHTreeItem::getSGObject: Error: %s\n", e.getDesc_cstr());
  }
  return sgObjectRTVal;
}

RTVal SHTreeItem::getSGObjectProperty() {
  RTVal sgObjectPropertyRTVal;
  try 
  {
    if(m_parentItem && m_parentItem->m_treeViewObjectDataRTVal.isValid() && 
       !m_parentItem->m_treeViewObjectDataRTVal.isNullObject()) 
    {
      RTVal args[2];
      args[0] = RTVal::ConstructUInt32(m_client, m_index.row());
      args[1] = RTVal::ConstructBoolean(m_client, false);//isValid
      sgObjectPropertyRTVal = m_parentItem->m_treeViewObjectDataRTVal.callMethod("SGObjectProperty", "getObjectProperty", 2, args);
      if(!args[1].getBoolean())
        sgObjectPropertyRTVal = RTVal();
    }
  }
  catch(Exception e) 
  {
    printf("SHTreeItem::getSGObjectProperty: Error: %s\n", e.getDesc_cstr());
  }
  return sgObjectPropertyRTVal;
}

void SHTreeItem::updateChildItemIfNeeded(int row) {
  if(m_childItems[row].m_updateNeeded) {
    m_childItems[row].m_updateNeeded = false;

    SHTreeItem *childItem = m_childItems[row].m_child.get();

    m_model->m_getUpdatedChildDataArgs[0] = RTVal::ConstructUInt32(m_client, row);//Size index (TODO: RTVal don't have method to update content for now...)
    m_model->m_getUpdatedChildDataArgs[1].setData(childItem);

    RTVal childDataRTVal =
      m_treeViewObjectDataRTVal.callMethod(
      "SGTreeViewObjectData",
      "getUpdatedChildData",
      7, m_model->m_getUpdatedChildDataArgs
     );

    bool invalidate = m_model->m_getUpdatedChildDataArgs[2].getBoolean();
    childItem->m_isPropagated = m_model->m_getUpdatedChildDataArgs[3].getBoolean();
    childItem->m_isOverride = m_model->m_getUpdatedChildDataArgs[4].getBoolean();
    childItem->m_isReference = m_model->m_getUpdatedChildDataArgs[5].getBoolean();
    childItem->m_isGenerator = m_model->m_getUpdatedChildDataArgs[6].getBoolean();
    childItem->updateNeeded(childDataRTVal, invalidate);
    if(invalidate) // Target object might have been created
      m_model->emitSceneHierarchyChanged();
  }
}

void SHTreeItem::updateNeeded() { 
  m_needsUpdate = true; 
}

void SHTreeItem::updateNeeded(RTVal treeViewObjectData, bool invalidate) {
  m_treeViewObjectDataRTVal = treeViewObjectData;
  if(invalidate) 
  {
    // TODO: remove all child nodes, recursively
    m_hadInitialUpdate = false;
  }
  m_needsUpdate = true;
}

void SHTreeItem::loadRecursively() {
  try
  {
    RTVal sgObject = getSGObject();
    if( sgObject.isValid() ) {
      RTVal arg = RTVal::ConstructBoolean( m_client, true );
      sgObject.callMethod( "", "forceHierarchyExpansion", 1, &arg );
    }
  }
  catch(Exception e) 
  {
    printf("Error: %s\n", e.getDesc_cstr());
  }
  m_model->emitSceneHierarchyChanged();
}

void SHTreeItem::updateChildItemsIfNeeded() {
  if(m_needsUpdate) 
  {
    if(m_treeViewObjectDataRTVal.isValid() && !m_treeViewObjectDataRTVal.isNullObject()) 
    {
      //std::cerr << "updateChildItemsIfNeeded " << (const char*)(desc().data()) << " " << std::endl;
      try 
      {
        //NOTE: normally we should be able to "reuse" the content m_updateArgs[2] but there seems to be a bug (getArraySize isn't refreshing)
        //A bug has been logged for this. To repro the bug, comment out the following line.
        m_model->m_updateArgs[2] = RTVal::Construct(m_client, "SGTreeViewObjectDataChanges", 0, 0);//io SGTreeViewObjectDataChanges changes
        unsigned int result = m_treeViewObjectDataRTVal.callMethod("UInt32", "update", 3, m_model->m_updateArgs).getUInt32();

        // If result is 0: no scene changes; no need to even recurse into children
        if(result != 0) 
        {
          if(result == 2) 
          {
            // Layout changed: we need to remove then insert rows
            RTVal removedItems = m_model->m_updateArgs[2].maybeGetMember("removed");
            unsigned int removedCount = removedItems.getArraySize();

            for(unsigned int i = 0; i < removedCount; ++i) 
            {
              unsigned int index = removedItems.getArrayElement(i).getUInt32();
              assert(index < m_childItems.size());
              m_model->beginRemoveRows(m_index, index, index);
              m_childItems.erase(m_childItems.begin() + index);
              m_model->endRemoveRows();
            }

            RTVal insertedItems = m_model->m_updateArgs[2].maybeGetMember("inserted");
            unsigned int insertedCount = insertedItems.getArraySize();

            if(insertedCount) 
            {
              RTVal insertedItemNames = m_model->m_updateArgs[2].maybeGetMember("insertedNames");

              for(unsigned int i = 0; i < insertedCount; ++i) {
                unsigned int index = insertedItems.getArrayElement(i).getUInt32();
                assert(index <= m_childItems.size());
                m_model->beginInsertRows(m_index, index, index);

                ChildItem childItem;
                childItem.m_child = new SHTreeItem(m_model, this, m_client);

                QString childName = insertedItemNames.getArrayElement(i).getStringCString();
                childItem.m_child->setName(childName);

                m_childItems.insert(m_childItems.begin() + index, childItem);

                m_model->endInsertRows();
              }
            }
            if(insertedCount != 0 || removedCount != 0) 
            {
              // Update childs' QModelIndex
              for(unsigned int i = 0; i < m_childItems.size(); ++i)
                m_childItems[i].m_child.get()->setIndex(m_model->createIndex(i, 0, m_childItems[i].m_child.get()));

              m_model->emitSceneHierarchyChanged();
            }
          }

          // All children might need to be updated
          unsigned int row = 0;
          for(ChildItemVec::iterator it = m_childItems.begin(); it != m_childItems.end(); ++it, ++row) 
          {
            it->m_updateNeeded = true;
            SHTreeItem *childItem = it->m_child.get();

            // Update child right away if it was already expanded
            // TODO: don't update if collapsed!
            if(childItem->m_hadInitialUpdate) 
            {
              updateChildItemIfNeeded(row);
              childItem->updateChildItemsIfNeeded();
            }
          }
        }
      }
      catch(Exception e) 
      {
        printf("Error: %s\n", e.getDesc_cstr());
      }
    } 
    else if(m_childItems.size()) 
    {
      // Ensure all children are deleted
      m_model->beginRemoveRows(m_index, 0, m_childItems.size()-1);
      m_childItems.clear();
      m_model->endRemoveRows();
    }
  }
  
  m_hadInitialUpdate = true;
  m_needsUpdate = false;
}

SHTreeItem *SHTreeItem::getOrCreateChildItem(int row) {
  updateChildItemsIfNeeded();
  updateChildItemIfNeeded(row);
  assert(row < int(m_childItems.size()));
  return m_childItems[row].m_child.get();
}
