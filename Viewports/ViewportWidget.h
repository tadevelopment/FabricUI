/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_VIEWPORT_H__
#define __FABRICUI_VIEWPORT_H__

#include <QtCore>
#include <QTime>
#include <QGLWidget>

namespace FabricUI
{
  namespace Viewports
  {
    class ViewportWidget : public QGLWidget
    { 
      private:
        void init();

      public:
        /// Constructor for RTRGLViewportWidget.
        ViewportWidget( 
          QGLContext *qglContext, 
          QWidget *parent = 0, 
          QGLWidget *share = 0);

        /// Constructor for GLViewportWidget.
      	ViewportWidget( 
          QGLFormat format, 
          QWidget *parent = 0);

      	virtual ~ViewportWidget() {};

        virtual bool isManipulationActive() { return false; }

        virtual void setManipulationActive(bool state) {}

        double fps() { return m_fps; }
       
      public slots:
        virtual void clear() {}
        void redraw() { update(); } 
        void toggleManipulation() { setManipulationActive(!isManipulationActive()); }

      protected:
        void computeFPS();
       
        double m_fps;
        bool m_hasCommercialLicense;
        double m_fpsStack[16];
     
        QTime m_fpsTimer;
   
    };
  }
}

#endif // __FABRICUI_VIEWPORT_H__
