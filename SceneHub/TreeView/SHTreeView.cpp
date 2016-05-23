/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHTreeView.h"
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QTreeWidgetItem>
#include <QtCore/QAbstractItemModel>
#include <FabricUI/SceneHub/Menus/SHContextualMenu.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;


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
  SHTreeItem *item = GetTreeItemAtIndex(indexAt(point));
  RTVal sgObject = RTVal();
  if(item) sgObject = item->getSGObject();
  SHContextualMenu *menu = new SHContextualMenu(m_shGLScene, m_shStates, sgObject, this, this);
  menu->constructMenu();
  menu->exec(mapToGlobal(point));
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
