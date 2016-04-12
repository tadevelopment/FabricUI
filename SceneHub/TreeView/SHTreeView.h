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
#include <FabricUI/SceneHub/TreeView/SHBaseTreeView.h>


namespace FabricUI {
namespace SceneHub {

class SHTreeView : public SHBaseTreeView {
  
  Q_OBJECT


  public:
    SHTreeView(FabricCore::Client &client, QWidget *parent = 0 );
        
    virtual ~SHTreeView() {}

    virtual void mousePressEvent(QMouseEvent *event);
    
    virtual void mouseDoubleClickEvent(QMouseEvent * event);
 

  signals:
    void itemSelected( FabricUI::SceneHub::SHTreeItem *item );
    
    void itemDeselected( FabricUI::SceneHub::SHTreeItem *item );
    
    void itemDoubleClicked( FabricUI::SceneHub::SHTreeItem *item );


  public slots:
    void onCustomContextMenu( const QPoint &point );


  protected:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
  

  private:
    QLineEdit *m_LineEdit;
};

} // namespace SceneHub
} // namespace FabricUI

#endif // __UI_SCENEHUB_SHTREEVIEW_H__
