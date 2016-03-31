/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHTREEITEM_H__
#define __UI_SCENEHUB_SHTREEITEM_H__

#include <QtCore/QAbstractItemModel>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <FabricCore.h>
#include <vector>
#include <assert.h>
#include <iostream>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHTreeModel;

    class SHTreeItem : public FTL::Shareable
    {
      struct ChildItem {
        bool m_updateNeeded;
        FTL::SharedPtr<SHTreeItem> m_child;

        ChildItem() 
          : m_updateNeeded(true) {}
      };

      typedef std::vector< ChildItem > ChildItemVec;

    public:

      enum ItemType { Object, Property, Operator };

      SHTreeItem(
        SHTreeModel *model,
        SHTreeItem *parentItem,
        FabricCore::Client client
        )
        : m_model( model )
        , m_parentItem( parentItem )
        , m_client( client )
        , m_needsUpdate( true )
        , m_hadInitialUpdate( false )
        , m_isPropagated( false )
        , m_isReference( false )
        , m_isGenerator( false )
      {
        if( !parentItem )
          m_isReference = true;
      }

      ~SHTreeItem() {}

      QString desc()
      {
        if( m_treeViewObjectDataRTVal.isValid() && !m_treeViewObjectDataRTVal.isNullObject() )
          return m_treeViewObjectDataRTVal.callMethod( "String", "getName", 0, 0 ).getStringCString();
        else
          return m_name;
      }

      void setIndex( QModelIndex index )
        { m_index = index; }

      QModelIndex getIndex()
        { return m_index; }

      void setName( QString name )
        { m_name = name; }

      void updateNeeded() {
        m_needsUpdate = true;
      }

      void updateNeeded( FabricCore::RTVal treeViewObjectData, bool invalidate ) {
        m_treeViewObjectDataRTVal = treeViewObjectData;
        if( invalidate ) {
          // TODO: remove all child nodes, recursively
          m_hadInitialUpdate = false;
        }
        m_needsUpdate = true;
      }

      SHTreeItem *parentItem()
        { return m_parentItem; }

      int childItemCount()
      {
        updateChildItemsIfNeeded();
        return m_childItems.size();
      }

      SHTreeItem *childItem( int row )
        { return getOrCreateChildItem( row ); }

      int childRow( SHTreeItem *childItem )
      {
        int row = 0;
        for( ChildItemVec::const_iterator it = m_childItems.begin(); it != m_childItems.end(); ++it, ++row ) {
          if( childItem == it->m_child.get() )
            return row;
        }
        assert( false );
        return 0;
      }

      void onSceneHierarchyChanged();

      FabricCore::RTVal getSGObject()
      {
        FabricCore::RTVal sgObjectRTVal;
        try
        {
          if( m_treeViewObjectDataRTVal.isValid() && !m_treeViewObjectDataRTVal.isNullObject() )
            sgObjectRTVal = m_treeViewObjectDataRTVal.callMethod("SGObject", "getObject", 0, 0);
          else if( m_parentItem && m_parentItem->m_treeViewObjectDataRTVal.isValid() && !m_parentItem->m_treeViewObjectDataRTVal.isNullObject() ) {
            // Might be a generator, in which case we return the generator owned container
            FabricCore::RTVal indexRTVal = FabricCore::RTVal::ConstructUInt32( m_client, m_index.row() );
            sgObjectRTVal = m_parentItem->m_treeViewObjectDataRTVal.callMethod( "SGObject", "getGeneratorOwnedObject", 1, &indexRTVal );
          }
        }
        catch ( FabricCore::Exception e )
        {
          printf("SHTreeItem::getSGObject: Error: %s\n", e.getDesc_cstr());
        }
        return sgObjectRTVal;
      }

      FabricCore::RTVal getSGObjectProperty() {
        FabricCore::RTVal sgObjectPropertyRTVal;
        try {
          if( m_parentItem && m_parentItem->m_treeViewObjectDataRTVal.isValid() && !m_parentItem->m_treeViewObjectDataRTVal.isNullObject() ) {
            FabricCore::RTVal indexRTVal = FabricCore::RTVal::ConstructUInt32( m_client, m_index.row() );
            sgObjectPropertyRTVal = m_parentItem->m_treeViewObjectDataRTVal.callMethod( "SGObjectProperty", "getObjectProperty", 1, &indexRTVal );
          }
        }
        catch( FabricCore::Exception e ) {
          printf( "SHTreeItem::getSGObjectProperty: Error: %s\n", e.getDesc_cstr() );
        }
        return sgObjectPropertyRTVal;
      }

      void loadRecursively();

      void updateChildItemsIfNeeded();

      bool isObject() const { return m_isReference; }
      bool isReference() const { return m_isReference; }
      bool isPropagated() const { return m_isPropagated; }
      bool isGenerator() const { return m_isGenerator; }

    protected:

      void updateChildItemIfNeeded( int row );

      SHTreeItem *getOrCreateChildItem( int row )
      {
        updateChildItemsIfNeeded();
        updateChildItemIfNeeded( row );
        assert( row < int(m_childItems.size()) );
        return m_childItems[row].m_child.get();
      }

    private:

      SHTreeModel *m_model;
      SHTreeItem *m_parentItem;
      FabricCore::Client m_client;
      FabricCore::RTVal m_treeViewObjectDataRTVal;
      ChildItemVec m_childItems;
      bool m_needsUpdate, m_hadInitialUpdate;
      QModelIndex m_index;
      QString m_name;

      bool m_isPropagated;
      bool m_isReference;
      bool m_isGenerator;
    };
  }
}

#endif // __UI_SCENEHUB_SHTREEITEM_H__
