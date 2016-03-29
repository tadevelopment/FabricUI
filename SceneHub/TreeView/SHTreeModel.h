/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHTREEMODEL_H__
#define __UI_SCENEHUB_SHTREEMODEL_H__

#include <QtCore/QAbstractItemModel>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <FabricCore.h>
#include <vector>
#include <assert.h>
#include <iostream>
#include <FabricUI/SceneHub/SHGLScene.h>

#include "SHTreeItem.h"

namespace FabricUI
{
  namespace SceneHub
  {
    class SHTreeModel : public QAbstractItemModel
    {
      Q_OBJECT

      friend class SHTreeItem;

      typedef std::vector<SHTreeItem *> RootItemsVec;

      public:
        SHTreeModel( SHGLScene *shGLScene, QObject *parent = 0 );

        SHTreeModel( FabricCore::Client client, FabricCore::RTVal sceneGraph, QObject *parent = 0 );

        ~SHTreeModel();

        QModelIndex addRootItemsFromScene(SHGLScene *shGLScene);

        QModelIndex addRootItem(FabricCore::RTVal rootSGObject);

        std::vector< QModelIndex > getIndicesFromSGObject( FabricCore::RTVal sgObject );

        virtual QVariant data(const QModelIndex &index, int role) const;

        virtual Qt::ItemFlags flags( const QModelIndex &index ) const;

        virtual QModelIndex index(int row, int col, const QModelIndex &parentIndex = QModelIndex() ) const;

        virtual QModelIndex parent( const QModelIndex &childIndex ) const;

        virtual int rowCount( const QModelIndex &index ) const;

        virtual int columnCount( const QModelIndex &index ) const { return 1; }

        class SceneHierarchyChangedBlocker {
          public:
            SceneHierarchyChangedBlocker(SHTreeModel *model) : m_model( model ) {
              if ( m_model->m_sceneHierarchyChangedBlockCount++ == 0 )
                m_model->m_sceneHierarchyChangedPending = false;
            }

            ~SceneHierarchyChangedBlocker() {
              if ( --m_model->m_sceneHierarchyChangedBlockCount == 0
                && m_model->m_sceneHierarchyChangedPending )
                emit m_model->sceneHierarchyChanged();
            }

          private:
            SHTreeModel *m_model;
        };

        void emitSceneHierarchyChanged() {
          if ( m_sceneHierarchyChangedBlockCount > 0 )
            m_sceneHierarchyChangedPending = true;
          else
            emit sceneHierarchyChanged();
        }



      signals:
        void sceneHierarchyChanged() const;


      public slots:
        void onSceneHierarchyChanged();


      private:
        RootItemsVec m_rootItems;
        FabricCore::Client m_client;
        FabricCore::RTVal m_treeViewDataRTVal;

        FabricCore::RTVal m_getUpdatedChildDataArgs[3];
        FabricCore::RTVal m_updateArgs[2];

        uint32_t m_sceneHierarchyChangedBlockCount;
        bool m_sceneHierarchyChangedPending;
    };
  }
}

#endif // __UI_SCENEHUB_SHTREEMODEL_H__
