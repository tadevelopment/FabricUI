/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_GL_VIEWPORT__
#define __UI_GL_VIEWPORT__

#include <FabricCore.h>
#include "ViewportWidget.h"
#include "ManipulationTool.h"

namespace FabricUI {
namespace Viewports {

class GLViewportWidget : public ViewportWidget
{
	Q_OBJECT

  friend class MainWindow;
 
  public:
  	GLViewportWidget(
      QColor bgColor, 
      QGLFormat format, 
      QWidget *parent = 0
      );

  	virtual ~GLViewportWidget();
    
    /// Implementation of ViewportWidget.
    virtual bool isManipulationActive();
    
    /// Implementation of ViewportWidget.
    virtual void setManipulationActive(
      bool state
      );
    
    /// Gets a pointor to the manip-tool.
    ManipulationTool* getManipTool();
    
    /// Gets tge camera.
    FabricCore::RTVal getCamera();
    
    /// Sets the grid visibility.
    void setGridVisible( 
      bool gridVisible, 
      bool update = true 
      );

    /// Checks if the grid is visible.
    bool isGridVisible();

  public slots:
    /// Implementation of ViewportWidget.
    virtual void clear();

    /// Implementation of ViewportWidget
    virtual bool onEvent(
      QEvent *event
      );

    /// Resets the camera to its 
    /// default parameters.
    void resetCamera();

  signals:
    /// Emitted when OpenGL has been initialized.
    void initComplete();
      
    /// Emitted after the vewport redraw.
    void redrawn();
        
    /// \internal, depreciated.
    void portManipulationRequested(
      QString portName
      );

  protected:
    /// Implementation of QGLWidget
    virtual void initializeGL();
    
    /// Implementation of QGLWidget
    virtual void resizeGL(
      int width, 
      int height
      );

    /// Implementation of QGLWidget
    virtual void paintGL();

  private:
    /// Sets the background color.
    void setBackgroundColor(
      QColor color
      );
      
    /// Initializes InlineDrawing.
    void initializeID( 
      bool shouldUpdateGL = true 
      );
    
    /// Manipulates the camera.
    bool manipulateCamera(
      FabricCore::RTVal klevent,
      bool shouldUpdateGL = true
      );

    QColor m_bgColor;
    bool m_resizedOnce;
    bool m_gridVisible;
 
    FabricCore::RTVal m_camera;
    FabricCore::RTVal m_drawing;
    FabricCore::RTVal m_viewport;
    ManipulationTool *m_manipTool;
    FabricCore::RTVal m_drawContext;
    FabricCore::RTVal m_cameraManipulator;
};

} // namespace Viewports
} // namespace FabricUI

#endif // __UI_GL_VIEWPORT__
