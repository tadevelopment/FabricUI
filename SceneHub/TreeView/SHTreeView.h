/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHTREEVIEW_H__
#define __UI_SCENEHUB_SHTREEVIEW_H__

#include <QtGui/QMenu>
#include <QtGui/QDrag>
#include <QtGui/QTreeView>
#include <QtGui/QLineEdit>
#include <QtGui/QMouseEvent>
 
#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <FabricUI/SceneHub/SHHotkeys.h>
#include <FabricUI/SceneHub/TreeView/SHTreeModel.h>


namespace FabricUI
{
  namespace SceneHub
  {
    class SHTreeView;

    class SHTreeView_ViewIndexTarget : public QObject
    {
      Q_OBJECT

      public:
        SHTreeView_ViewIndexTarget(SHTreeView *view, QModelIndex const &index, QObject *parent)
          : QObject( parent )
          , m_view( view )
          , m_index( index )
          {}

        void setVisibility( bool visible, unsigned char propagationType );

      public slots:
        void expandRecursively() { expandRecursively( m_index ); }
        void loadRecursively();

        void showLocal() { setVisibility( true, 0 ); }
        void showPropagated() { setVisibility( true, 1 ); }
        void showOverride() { setVisibility( true, 2 ); }

        void hideLocal() { setVisibility( false, 0 ); }
        void hidePropagated() { setVisibility( false, 1 ); }
        void hideOverride() { setVisibility( false, 2 ); }

      protected:
        void expandRecursively( QModelIndex const &index );

      private:
        SHTreeView *m_view;
        QModelIndex m_index;
    };

    class SHTreeView : public QTreeView
    {
      Q_OBJECT

        friend class SHTreeView_ViewIndexTarget;

      public:
        SHTreeView(FabricCore::Client &client, QWidget *parent = 0 );
        void setSelectedObjects( FabricCore::RTVal selectedSGObjectArray );
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseDoubleClickEvent(QMouseEvent * event);
     
      signals:
        void itemSelected( FabricUI::SceneHub::SHTreeItem *item );
        void itemDeselected( FabricUI::SceneHub::SHTreeItem *item );
        void itemDoubleClicked( FabricUI::SceneHub::SHTreeItem *item );
        void addExternalFile(QStringList, bool);

      public slots:
        void onCustomContextMenu( const QPoint &point );

      protected:
        void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
      
      private:
        /// \internal
        /// Reference to the client -> construct RTVal.
        FabricCore::Client m_client;
        QLineEdit *m_LineEdit;
    };
  }
}

#endif // __UI_SCENEHUB_SHTREEVIEW_H__
