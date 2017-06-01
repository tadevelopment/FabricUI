/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_GLVIEWPORT_H__
#define __FABRICUI_GLVIEWPORT_H__

#include <FabricCore.h>
#include "ViewportWidget.h"
#include "ManipulationTool.h"

namespace FabricUI
{
  namespace Viewports
  {
    class GLViewportWidget;

    class GLViewportWidgetEventFilter : public QObject 
    {
      Q_OBJECT

      public:
        GLViewportWidgetEventFilter(
          GLViewportWidget *viewport
          );

        bool eventFilter(
          QObject *object, 
          QEvent *event
          );

      private:
        GLViewportWidget *m_viewport;
    };

    class GLViewportWidget : public ViewportWidget
    {
    	Q_OBJECT

      friend class MainWindow;

    public:
    	GLViewportWidget(
        QColor bgColor, 
        QGLFormat format, 
        QWidget *parent = NULL);

    	virtual ~GLViewportWidget();
      
      virtual bool isManipulationActive();
      
      virtual void setManipulationActive(bool state);
     
      ManipulationTool * getManipTool() { return m_manipTool; }
      
      virtual FabricCore::RTVal getCamera() { return m_camera; }
      
      void setGridVisible( bool gridVisible, bool update = true );

      bool isGridVisible();
 
    public slots:
      virtual void clear();

      virtual bool onEvent(QEvent *event);

      void resetCamera();
 
    signals:
      void dirty();
      
      void initComplete();
      
      void redrawn();
      
      void portManipulationRequested(QString portName);

    protected:
      /// Implementation of QGLWidget
      virtual void initializeGL();
      
      /// Implementation of QGLWidget
      virtual void resizeGL(int w, int h);

      /// Implementation of QGLWidget
      virtual void paintGL();

      
    private:
      void setBackgroundColor(QColor color);
      
      void resetRTVals( bool shouldUpdateGL = true );
      
      bool manipulateCamera(
        FabricCore::RTVal klevent,
        bool shouldUpdateGL = true);
 
      bool m_resizedOnce;
      bool m_gridVisible;
      QColor m_bgColor;

      FabricCore::RTVal m_drawing;
      FabricCore::RTVal m_drawContext;
      FabricCore::RTVal m_cameraManipulator;
      ManipulationTool *m_manipTool;
      FabricCore::RTVal m_viewport;
      FabricCore::RTVal m_camera;

      GLViewportWidgetEventFilter *m_eventFilter;
    };

  }
}

#endif // __FABRICUI_GLVIEWPORT_H__
