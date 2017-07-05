/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_VIEWPORT__
#define __UI_VIEWPORT__

#include <QTime>
#include <QGLWidget>
#include "ViewportEventFilter.h"

namespace FabricUI {
namespace Viewports {

class ViewportWidget : public QGLWidget
{ 
  Q_OBJECT
  
  public:
    /// Constructor for RTRGLViewportWidget.
    ViewportWidget( 
      QGLContext *qglContext, 
      QWidget *parent = 0, 
      QGLWidget *share = 0
      );

    /// Constructor for GLViewportWidget.
  	ViewportWidget( 
      QGLFormat format, 
      QWidget *parent = 0
      );

  	virtual ~ViewportWidget();

    /// Check if the manipulation is activated.
    /// To override
    virtual bool isManipulationActive();

    /// Activate/deactivaye the manips.
    /// To override
    virtual void setManipulationActive(
      bool state
      );

    /// Gets the FPS.
    double fps();
  
  signals:
    /// Emitted when OpenGL has been initialized.
    void initComplete();
      
    /// Emitted after the vewport redraw.
    void redrawn();

  public slots:
    /// Clear the widget.
    /// To override
    virtual void clear();

    /// Implementation of QGLWidget
    virtual bool onEvent(
      QEvent *event
      );

    /// Refreshs opengl.
    void redraw();
        
    /// Toggles the manipulation
    void toggleManipulation();

  protected:
    /// Initializes the viewport.
    /// Callded from constructors.
    void init();

    /// Compute the frame-rate.
    void computeFPS();
   
    double m_fps;
    QTime m_fpsTimer;
    double m_fpsStack[16];
    ViewportEventFilter *m_eventFilter;
};
  
} // namespace Viewports
} // namespace FabricUI

#endif // __UI_VIEWPORT__
