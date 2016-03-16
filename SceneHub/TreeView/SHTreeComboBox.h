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
          //std::cerr << "showPopup " << std::endl;
          //while (count() > 0) { removeItem(0); }
          //addItem("hello");
          //addItem("World"):
          emit updateList();
          QComboBox::showPopup();
        }

      signals:
        void updateList();
    };
  }
}

#endif // __UI_SCENEHUB_SHTREE_COMBOBOX_H__
