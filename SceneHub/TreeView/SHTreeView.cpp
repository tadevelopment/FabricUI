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


void SHTreeView_ViewIndexTarget::expandRecursively( QModelIndex const &index ) {
  SHTreeModel *model = const_cast<SHTreeModel *>(
    static_cast<SHTreeModel const *>( index.model() )
    );
  SHTreeModel::SceneHierarchyChangedBlocker blocker( model );

  m_view->expand( index );
  int rows = model->rowCount( index );
  for ( int row = 0; row < rows; ++row ) {
    QModelIndex childIndex = model->index( row, 0, index );
    expandRecursively( childIndex );
  }
}

void SHTreeView_ViewIndexTarget::loadRecursively() {
  SHTreeModel *model = const_cast<SHTreeModel *>(
    static_cast<SHTreeModel const *>( m_index.model() )
    );
  SHTreeModel::SceneHierarchyChangedBlocker blocker( model );

  SHTreeItem *item = static_cast<SHTreeItem *>( m_index.internalPointer() );
  item->loadRecursively();
}

void SHTreeView_ViewIndexTarget::setVisibility( bool visible, unsigned char propagationType ) {
  SHTreeItem *item = static_cast<SHTreeItem *>( m_index.internalPointer() );
  FabricCore::RTVal sgObjectVal = item->getSGObject();
  if( sgObjectVal.isValid() ) {

    SHTreeModel *model = const_cast<SHTreeModel *>(
      static_cast<SHTreeModel const *>( m_index.model() )
      );

    try {
      FabricCore::RTVal args[2];
      args[0] = FabricCore::RTVal::ConstructBoolean( m_view->m_client, visible );
      args[1] = FabricCore::RTVal::ConstructUInt8( m_view->m_client, propagationType );
      sgObjectVal.callMethod( "", "setVisibility", 2, args );
    }
    catch( FabricCore::Exception e ) {
      printf( "SHTreeView_ViewIndexTarget::setVisibility: Error: %s\n", e.getDesc_cstr() );
    }
    model->emitSceneChanged();
  }
}

// *********

SHTreeView::SHTreeView(FabricCore::Client &client, QWidget *parent) : QTreeView( parent ) {
  m_client = client;
  this->setAcceptDrops(true);

  setHeaderHidden( true );
  setSelectionMode( QAbstractItemView::SingleSelection );
  setContextMenuPolicy( Qt::CustomContextMenu );
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)) );
}

void SHTreeView::setSelectedObjects( FabricCore::RTVal selectedSGObjectArray ) {

  SHTreeModel* treeModel = (SHTreeModel*)model();

  try {
    std::vector< QModelIndex > selectedIndices;

    unsigned int count = selectedSGObjectArray.getArraySize();
    for( unsigned int i = 0; i < count; ++i ) {
      FabricCore::RTVal sgObject = selectedSGObjectArray.getArrayElement( i );
      std::vector< QModelIndex > indices = treeModel->getIndicesFromSGObject( sgObject );
      selectedIndices.insert( selectedIndices.end(), indices.begin(), indices.end() );
    }

    if( selectedIndices.empty() ) clearSelection();
    else {
      QItemSelectionModel* selection = selectionModel();
      for( unsigned int i = 0; i < selectedIndices.size(); ++i )
        selection->select( selectedIndices[i], i == 0 ? (QItemSelectionModel::Current | QItemSelectionModel::Clear | QItemSelectionModel::Select) : QItemSelectionModel::Select );
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf("SHTreeModel::setSelectedObjects: Error: %s\n", e.getDesc_cstr());
  }
}

void SHTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
  QTreeView::selectionChanged( selected, deselected );
  foreach ( QModelIndex index, deselected.indexes() ) {
    SHTreeItem *item = static_cast<SHTreeItem *>( index.internalPointer() );
    emit itemDeselected( item );
  }

  foreach ( QModelIndex index, selected.indexes() ) {
    SHTreeItem *item = static_cast<SHTreeItem *>( index.internalPointer() );
    emit itemSelected( item );
  }
}

void SHTreeView::onCustomContextMenu( const QPoint &point ) {
  QModelIndex index = indexAt( point );
  if ( index.isValid() )
  {
    SHTreeItem *item = static_cast<SHTreeItem *>( index.internalPointer() );
    FabricCore::RTVal sgObjectVal = item->getSGObject();
    if( sgObjectVal.isValid() ) {
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
      /* THERE ARE BUGS WITH OVERRIDES
      visAction = new QAction( "Show (override)", 0 );
      connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( showOverride() ) );
      visAction->setCheckable( true );
      if( visible && propagType == 2 )
        visAction->setChecked( true );
      visMenu->addAction( visAction );
      */
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
      /* THERE ARE BUGS WITH OVERRIDES
      visAction = new QAction( "Hide (override)", 0 );
      connect( visAction, SIGNAL( triggered() ), viewIndexTarget, SLOT( hideOverride() ) );
      visAction->setCheckable( true );
      if( !visible && propagType == 2 )
        visAction->setChecked( true );
      visMenu->addAction( visAction );
      */

      menu->popup( mapToGlobal( point ) );
    }
  }
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
 
void SHTreeView::mouseDoubleClickEvent(QMouseEvent * event)
{
  QTreeView::mousePressEvent(event);

  QList<QModelIndex> modelIndexList = selectedIndexes();
  foreach( QModelIndex index, modelIndexList )
  {
    SHTreeItem *item = static_cast<SHTreeItem *>( index.internalPointer() );
    if( !item )
      continue;
    emit itemDoubleClicked( item );
  }
}
