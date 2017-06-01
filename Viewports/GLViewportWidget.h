/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_GLVIEWPORT_H__
#define __FABRICUI_GLVIEWPORT_H__


#include <FabricCore.h>
#include "ViewportWidget.h"

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
          QObject *, 
          QEvent *event
          );

      private:
        GLViewportWidget *m_viewport;
    };

    class GLViewportWidget : public ViewportWidget
    {
    	Q_OBJECT

      friend class MainWindow;
      friend class ManipulationTool;
      friend class GLViewportWidgetEventFilter;

    public:
    	GLViewportWidget(
        QColor bgColor, 
        QGLFormat format, 
        QWidget *parent = NULL);

    	virtual ~GLViewportWidget();
      
      virtual void setBackgroundColor(QColor color);
      virtual bool isManipulationActive();
      virtual void setManipulationActive(bool state);
      virtual void clearInlineDrawing();
      QColor backgroundColor() { return m_bgColor; }
     
      
      FabricCore::RTVal getDrawContext() { return m_drawContext; }
      virtual ManipulationTool * getManipTool() { return m_manipTool; }
      virtual FabricCore::RTVal getCameraManipulator() { return m_cameraManipulator; }
      virtual FabricCore::RTVal getCamera() { return m_camera; }
      void updateFromManip() {  emit dirty(); }
      bool isGridVisible();
 
    public slots:
      void toggleManipulation() { setManipulationActive(!isManipulationActive()); }
      void setGridVisible( bool gridVisible, bool update = true );
      void resetCamera();
 
    signals:
      void dirty();
      void initComplete();
      void redrawn();
      void portManipulationRequested(QString portName);

    protected:
      virtual void initializeGL();
      virtual void resizeGL(int w, int h);
      virtual void paintGL();
      virtual bool onEvent(QEvent *event);
      
      void resetRTVals( bool shouldUpdateGL = true );
      bool manipulateCamera(
        QEvent *event,
        bool shouldUpdateGL = true);
 
      int m_width;
      int m_height;
      bool m_resizedOnce;
      bool m_gridVisible;
      QColor m_bgColor;

      FabricCore::RTVal m_drawing;
      FabricCore::RTVal m_drawContext;
      FabricCore::RTVal m_camera;
      FabricCore::RTVal m_cameraManipulator;
      ManipulationTool *m_manipTool;
      FabricCore::RTVal m_viewport;

      GLViewportWidgetEventFilter *m_eventFilter;
    };

  }
}

#include "ManipulationTool.h"

#endif // __FABRICUI_GLVIEWPORT_H__
