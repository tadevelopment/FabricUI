/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_VIEWPORT__
#define __UI_VIEWPORT__

#include <QTime>
#include <QGLWidget>

namespace FabricUI {
namespace Viewports {

class ViewportWidget : public QGLWidget
{ 
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
   
  public slots:
    /// Clear the widget.
    /// To override
    virtual void clear();

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
};
  
}
}

#endif // __UI_VIEWPORT__
