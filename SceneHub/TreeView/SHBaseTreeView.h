/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHBASETREEVIEW_H__
#define __UI_SCENEHUB_SHBASETREEVIEW_H__

#include <QtGui/QMenu>
#include <QtGui/QDrag>
#include <QtGui/QTreeView>
#include <QtGui/QMouseEvent>
#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <FabricUI/SceneHub/SHHotkeys.h>
#include <FabricUI/SceneHub/SHGLScene.h>
#include <FabricUI/SceneHub/TreeView/SHTreeModel.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHBaseTreeView;

    class SHTreeView_ViewIndexTarget : public QObject {
      Q_OBJECT

      public:
        SHTreeView_ViewIndexTarget(SHBaseTreeView *view, QModelIndex const &index, QObject *parent);


      public slots:
        void expandRecursively() { expandRecursively( m_index ); }
        
        void loadRecursively();

      protected:
        void expandRecursively( QModelIndex const &index );

      private:
        SHBaseTreeView *m_view;
        QModelIndex m_index;
    };

    class SHBaseTreeView : public QTreeView {
      Q_OBJECT

      public:
        SHBaseTreeView(FabricCore::Client &client, QWidget *parent = 0 );

        virtual ~SHBaseTreeView() {}

        void setSelectedObjects( SHGLScene *scene );

        void setSelectedObjects( FabricCore::RTVal selectedSGObjectArray );

        static SHTreeItem *GetTreeItemAtIndex(QModelIndex index);


      protected:
        SHTreeItem *getTreeItemAtIndex(QModelIndex index);

        /// \internal
        /// Reference to the client -> construct RTVal.
        FabricCore::Client m_client;
    };
  }
}

#endif // __UI_SCENEHUB_SHBASETREEVIEW_H__
