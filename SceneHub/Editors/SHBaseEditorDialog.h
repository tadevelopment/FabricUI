/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHBaseEditorDialog_H__
#define __UI_SCENEHUB_SHBaseEditorDialog_H__

#include <QtGui/QColor>
#include <QtGui/QWidget>
#include <FabricUI/SceneHub/SHGLScene.h>
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SHBaseEditorDialog : public QWidget
    {
      Q_OBJECT

      public:
        SHBaseEditorDialog(QWidget*parent, SHGLScene *shGLScene);

        virtual ~SHBaseEditorDialog() {};

      protected:
        virtual void mouseMoveEvent(QMouseEvent *event) {};
        
        virtual void mouseReleaseEvent(QMouseEvent *event) {};
        
        virtual void closeEvent(QCloseEvent *event);
        
        virtual void showColorDialog();

        QColor m_color;
        SHGLScene *m_shGLScene;
    };
  }
}

#endif // __UI_SCENEHUB_SHBaseEditorDialog_H__
