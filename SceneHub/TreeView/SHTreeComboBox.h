/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHTREE_COMBOBOX_H__
#define __UI_SCENEHUB_SHTREE_COMBOBOX_H__

#include <QtGui/QMenu>
#include <QtGui/QComboBox>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHTreeComboBox : public QComboBox
    {
      Q_OBJECT 
      public:
        SHTreeComboBox(QWidget *parent = 0) : QComboBox(parent){}
        virtual ~SHTreeComboBox() {}
 
        virtual void showPopup() {
          emit updateList();
          QComboBox::showPopup();
        }

      signals:
        void updateList();
    };
  }
}

#endif // __UI_SCENEHUB_SHTREE_COMBOBOX_H__
