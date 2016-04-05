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
 
 
SHTreeView::SHTreeView(FabricCore::Client &client, QWidget *parent) : SHBaseTreeView(client, parent ) {
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

void SHTreeView::mousePressEvent(QMouseEvent *event) {

  QList<QModelIndex> modelIndexList = selectedIndexes();

  try 
  {
    foreach( QModelIndex index, modelIndexList )
    {
      SHTreeItem *item = static_cast<SHTreeItem *>( index.internalPointer() );
      FabricCore::RTVal sgObject = item->getSGObject();
      if( sgObject.isValid() ) {
        FabricCore::RTVal sgParent = sgObject.callMethod( "SGObject", "getOwnerInstance", 0, 0 );

        if( sgParent.callMethod( "Boolean", "isValid", 0, 0 ).getBoolean() ) {
          FabricCore::RTVal parentNameVal = sgParent.callMethod( "String", "getName", 0, 0 );
          FabricCore::RTVal typeVal = sgObject.callMethod( "String", "type", 0, 0 );
          std::string parentName = std::string( parentNameVal.getStringCString() );
          std::string type = std::string( typeVal.getStringCString() );

          // Assets
          if( parentName.compare( SH_ASSETS_LIBRARY ) == 0 ) {
            FabricCore::RTVal param = FabricCore::RTVal::ConstructString( m_client, "path" );
            FabricCore::RTVal sgProperty = sgObject.callMethod( "SGObjectProperty", "getLocalProperty", 1, &param );

            if( sgProperty.callMethod( "Boolean", "isValid", 0, 0 ).getBoolean() ) {
              sgProperty.callMethod( "", "getValue", 1, &param );
              // Create data
              QMimeData *mimeData = new QMimeData();
              QList<QUrl> urlsList;

              QString url( std::string( std::string( "file://" ) + std::string( param.getStringCString() ) ).c_str() );
              urlsList.push_back( QUrl( url ) );
              mimeData->setUrls( urlsList );
              // Create drag
              QDrag *drag = new QDrag( this );
              drag->setMimeData( mimeData );
              drag->exec( Qt::CopyAction );
            }
          }
          break;
        }
      }
    }
  }

  catch ( FabricCore::Exception e )
  {
    printf("SHTreeView::mousePressEvent: Error: %s\n", e.getDesc_cstr());
  }

  QTreeView::mousePressEvent(event);
}

void SHTreeView::mouseDoubleClickEvent(QMouseEvent * event) {
  QTreeView::mousePressEvent(event);

  QList<QModelIndex> modelIndexList = selectedIndexes();
  foreach( QModelIndex index, modelIndexList )
  {
    SHTreeItem *item = static_cast<SHTreeItem *>( index.internalPointer() );
    if( !item ) continue;
    emit itemDoubleClicked( item );
  }
}
