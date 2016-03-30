/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHTreeView.h"
  
using namespace FabricUI;
using namespace FabricUI::SceneHub;
 
 
SHTreeView::SHTreeView(FabricCore::Client &client, QWidget *parent)
  : SHBaseTreeView(client, parent ) 
{
  setHeaderHidden( true );
  setSelectionMode( QAbstractItemView::SingleSelection );
  setContextMenuPolicy( Qt::CustomContextMenu );
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)) );
}

void SHTreeView::onCustomContextMenu(const QPoint &point) {
  QModelIndex index = indexAt( point );
  if ( index.isValid() )
  {
    QMenu *menu = new QMenu(this);

    SHTreeView_ViewIndexTarget *viewIndexTarget =  new SHTreeView_ViewIndexTarget( this, index, menu );

    QAction *expandAction = new QAction( "Expand recursively", 0 );
    connect( expandAction, SIGNAL(triggered()), viewIndexTarget, SLOT(expandRecursively()) );
    menu->addAction( expandAction );

    QAction *loadAction = new QAction( "Load recursively", 0 );
    connect(loadAction, SIGNAL(triggered()), viewIndexTarget, SLOT(loadRecursively()) );
    menu->addAction( loadAction );

    menu->popup( mapToGlobal( point ) );
  }
}
 
void SHTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
  QTreeView::selectionChanged(selected, deselected);
  foreach (QModelIndex index, deselected.indexes()) 
    emit itemDeselected( getTreeItemAtIndex(index));

  foreach (QModelIndex index, selected.indexes()) 
    emit itemSelected(getTreeItemAtIndex(index));
}
