/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHTREEVIEW_H__
#define __UI_SCENEHUB_SHTREEVIEW_H__

#include "SHBaseTreeView.h"
#include <FabricUI/SceneHub/SHHotkeys.h>
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SHTreeView : public SHBaseTreeView {
      Q_OBJECT

      public:
        SHTreeView(FabricCore::Client &client, QWidget *parent = 0);

        virtual void mousePressEvent(QMouseEvent *event);
        
        virtual void mouseDoubleClickEvent(QMouseEvent * event);
     

      signals:
        virtual void itemSelected(FabricUI::SceneHub::SHTreeItem *item);
        
        virtual void itemDeselected(FabricUI::SceneHub::SHTreeItem *item);
        
        virtual void itemDoubleClicked( FabricUI::SceneHub::SHTreeItem *item );
        

      public slots:
        void onCustomContextMenu(const QPoint &point);


      protected:
        void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
      
        QLineEdit *m_LineEdit;
    };
  }
}

#endif // __UI_SCENEHUB_SHTREEVIEW_H__
