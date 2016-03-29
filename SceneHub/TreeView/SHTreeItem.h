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

namespace FabricUI {

  namespace SceneHub {

    class SHTreeModel;

    class SHTreeItem : public FTL::Shareable {

      struct ChildItem {
        bool m_updateNeeded;
        FTL::SharedPtr<SHTreeItem> m_child;
        ChildItem() : m_updateNeeded(true) {}
      };

      typedef std::vector< ChildItem > ChildItemVec;


      public:
        SHTreeItem(SHTreeModel *model, SHTreeItem *parentItem, FabricCore::Client client);

        ~SHTreeItem() {}

        QString desc();

        void setIndex( QModelIndex index ) { m_index = index; }

        QModelIndex getIndex() { return m_index; }

        void setName( QString name ) { m_name = name; }

        void updateNeeded() { m_needsUpdate = true; }

        SHTreeItem *parentItem()  { return m_parentItem; }

        SHTreeItem *childItem( int row ) { return getOrCreateChildItem( row ); }

        void updateNeeded( FabricCore::RTVal treeViewObjectData, bool invalidate );

        int childItemCount();

        int childRow( SHTreeItem *childItem );

        FabricCore::RTVal getSGObject();
    
        void loadRecursively();

        void updateChildItemsIfNeeded();


      protected:
        void updateChildItemIfNeeded( int row );

        SHTreeItem *getOrCreateChildItem( int row );


      private:
        SHTreeModel *m_model;
        SHTreeItem *m_parentItem;
        FabricCore::Client m_client;
        FabricCore::RTVal m_treeViewObjectDataRTVal;
        ChildItemVec m_childItems;
        bool m_needsUpdate, m_hadInitialUpdate;
        QModelIndex m_index;
        QString m_name;
      };

  }
}

#endif // __UI_SCENEHUB_SHTREEITEM_H__
