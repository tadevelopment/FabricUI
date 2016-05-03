/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHTreeView.h"
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QTreeWidgetItem>
#include <QtCore/QAbstractItemModel>
 
using namespace FabricUI;
using namespace FabricUI::SceneHub;


SHTreeView::SHTreeView(
  FabricCore::Client client,
  SHStates *shStates, 
  SHGLScene *shGLScene)
  : SHBaseTreeView(client)
  , m_shStates(shStates)
  , m_shGLScene(shGLScene)
{
  setHeaderHidden(true);
  setContextMenuPolicy(Qt::CustomContextMenu);
  setSelectionMode( QAbstractItemView::ExtendedSelection);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
}

void SHTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
  // clear selection (make sure 3D view is synchronized) if all elements are newly added
  bool clear = QTreeView::selectionModel()->selectedIndexes().size() == selected.indexes().size();
  QTreeView::selectionChanged(selected, deselected);
       
  foreach (QModelIndex index, deselected.indexes()) 
  {
    SHTreeItem *item = SHBaseTreeView::GetTreeItemAtIndex(index);
    emit itemDeselected(item);
  }

  if (clear)
    emit selectionCleared();

  foreach (QModelIndex index, selected.indexes()) 
  {
    SHTreeItem *item = SHBaseTreeView::GetTreeItemAtIndex(index);
    emit itemSelected(item);
  }
}

void SHTreeView::onCustomContextMenu(const QPoint &point) {
  QModelIndex index = indexAt(point);
  /*
  if (index.isValid())
  {
    SHTreeItem *item = static_cast<SHTreeItem *>(index.internalPointer());
    FabricCore::RTVal sgObjectVal = item->getSGObject();
    if(sgObjectVal.isValid()) 
    {
      //NOTE: all this is probably leaking... these should at least be reused
      QMenu *menu = new QMenu(this);

      SHTreeView_ViewIndexTarget *viewIndexTarget =
        new SHTreeView_ViewIndexTarget( this, index, menu );

      QAction *expandAction = new QAction( "Expand recursively", 0 );
      connect( expandAction, SIGNAL(triggered()), viewIndexTarget, SLOT(expandRecursively()) );
      menu->addAction( expandAction );

      QAction *loadAction = new QAction( "Load recursively", 0 );
      connect(loadAction, SIGNAL(triggered()), viewIndexTarget, SLOT(loadRecursively()) );
      menu->addAction( loadAction );

      bool visible = false;
      unsigned char propagType = 0;
      try {
        FabricCore::RTVal propagVal = FabricCore::RTVal::ConstructUInt8( m_client, 0 );
        visible = sgObjectVal.callMethod( "Boolean", "getVisibility", 1, &propagVal ).getBoolean();
        propagType = propagVal.getUInt8();
      }
      catch( FabricCore::Exception e ) {
        printf( "SHTreeView::onCustomContextMenu: Error: %s\n", e.getDesc_cstr() );
      }

      QMenu *visMenu = menu->addMenu( tr( "Visibility" ) );

      QAction *visAction = new QAction( "Show", 0 );
      connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( showLocal() ) );
      visAction->setCheckable( true );
      if( visible && propagType == 0 )
        visAction->setChecked( true );
      visMenu->addAction( visAction );

      visAction = new QAction( "Show (propagated)", 0 );
      connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( showPropagated() ) );
      visAction->setCheckable( true );
      if( visible && propagType == 1 )
        visAction->setChecked(true);
      visMenu->addAction( visAction );

      // THERE ARE BUGS WITH OVERRIDES
      //visAction = new QAction( "Show (override)", 0 );
      //connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( showOverride() ) );
      //visAction->setCheckable( true );
      //if( visible && propagType == 2 )
      //  visAction->setChecked( true );
      //visMenu->addAction( visAction );
      //

      visAction = new QAction( "Hide", 0 );
      connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( hideLocal() ) );
      visAction->setCheckable( true );
      if( !visible && propagType == 0 )
        visAction->setChecked( true );
      visMenu->addAction( visAction );

      visAction = new QAction( "Hide (propagated)", 0 );
      connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( hidePropagated() ) );
      visAction->setCheckable( true );
      if( !visible && propagType == 1 )
        visAction->setChecked( true );
      visMenu->addAction( visAction );

      // THERE ARE BUGS WITH OVERRIDES
      //visAction = new QAction( "Hide (override)", 0 );
      //connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( hideOverride() ) );
      //visAction->setCheckable( true );
      //if( !visible && propagType == 2 )
      //  visAction->setChecked( true );
      //visMenu->addAction( visAction );
      //

      menu->popup( mapToGlobal( point ) );
    }
  }
  */
}
 
void SHTreeView::mouseDoubleClickEvent(QMouseEvent *event) {
  QList<QModelIndex> modelIndexList = selectedIndexes();
  foreach(QModelIndex index, modelIndexList)
  {
    SHTreeItem *item = GetTreeItemAtIndex(index);
    if(item)
      emit itemDoubleClicked(item);
  }
}
