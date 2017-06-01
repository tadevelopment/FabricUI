/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include <iostream>
#include <QMouseEvent>
#include <QApplication>
#include "QtToKLEvent.h"
#include "GLViewportWidget.h"
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Viewports;
using namespace FabricCore;
using namespace Application;

GLViewportWidgetEventFilter::GLViewportWidgetEventFilter(
  GLViewportWidget *viewport)
 : QObject()
 , m_viewport(viewport) 
{
}

bool GLViewportWidgetEventFilter::eventFilter(
  QObject *, 
  QEvent *event) 
{
  if( event->type() == QEvent::ShortcutOverride ||
      event->type() == QEvent::KeyRelease ||
      event->type() == QEvent::MouseButtonPress ||
      event->type() == QEvent::MouseButtonDblClick ||
      event->type() == QEvent::MouseButtonRelease ||
      event->type() == QEvent::Wheel ||
      event->type() == QEvent::MouseMove) 
  {
    m_viewport->onEvent(event);
  }

  return false;
}

GLViewportWidget::GLViewportWidget(
  QColor bgColor, 
  QGLFormat format, 
  QWidget *parent)
  : ViewportWidget(format, parent)
  , m_bgColor(bgColor)
{	
  m_manipTool = new ManipulationTool();
  setAutoBufferSwap(false);

  m_eventFilter = new GLViewportWidgetEventFilter(this);
  installEventFilter(m_eventFilter);

  m_gridVisible = true; // default value
  
  QSettings* settings = FabricApplicationStates::GetAppStates()->getSettings();
  if(settings && settings->contains("glviewport/gridVisible"))
    m_gridVisible = settings->value("glviewport/gridVisible").toBool();

  try
  {
    Client client = FabricApplicationStates::GetAppStates()->getClient();
    client.loadExtension("Manipulation", "", false);
    client.loadExtension("InlineDrawing", "", false);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "GLViewportWidget::GLViewportWidget",
      "",
      e.getDesc_cstr());
  }
  
  m_resizedOnce  = false;
  resetRTVals( false /*shouldUpdateGL*/ );
}

GLViewportWidget::~GLViewportWidget()
{
  delete(m_manipTool);
}

bool GLViewportWidget::onEvent(QEvent *event)
{
  bool redrawRequested;
  QString manipulatedPortName;

  bool alt = QApplication::keyboardModifiers().testFlag(Qt::AltModifier);
  
  bool isAccepted = alt 
    ? false
    : m_manipTool->onEvent(
        event,
        m_viewport,
        redrawRequested,
        manipulatedPortName);

  if(redrawRequested)
    updateGL();

  if(!manipulatedPortName.isEmpty())
    emit portManipulationRequested(manipulatedPortName);
  
  if(alt && !isAccepted)
    isAccepted = manipulateCamera(event);
 
  if(!event->isAccepted() && isAccepted)
    event->setAccepted(isAccepted);

  return isAccepted;
}

void GLViewportWidget::setBackgroundColor(QColor color)
{
  m_bgColor = color;

  if(m_viewport.isValid())
  {
    try
    {
      Context context = FabricApplicationStates::GetAppStates()->getContext();
      RTVal bgColorVal = m_viewport.maybeGetMember("bgColor");
      bgColorVal.setMember("r", RTVal::ConstructFloat32(context, float(m_bgColor.red()) / 255.0f));
      bgColorVal.setMember("g", RTVal::ConstructFloat32(context, float(m_bgColor.green()) / 255.0f));
      bgColorVal.setMember("b", RTVal::ConstructFloat32(context, float(m_bgColor.blue()) / 255.0f));
      m_viewport.setMember("bgColor", bgColorVal);
    }

    catch(Exception &e)
    {
      FabricException::Throw(
        "GLViewportWidget::setBackgroundColor",
        "",
        e.getDesc_cstr());
    }
  }
}

bool GLViewportWidget::isManipulationActive() 
{
  return m_manipTool->isActive();
}

void GLViewportWidget::setManipulationActive(bool state)
{
  if(state == m_manipTool->isActive())
    return;

  if(state)
  {
    m_manipTool->toolOnSetup();
    setFocus();
  }
  else
  {
    clearFocus();
    m_manipTool->toolOffCleanup();
  }

  setMouseTracking(state);
  redraw();
}

void GLViewportWidget::clearInlineDrawing()
{
  if(m_viewport.isValid() && m_drawContext.isValid())
  {
    resetRTVals();
    initializeGL();
    resizeGL(size().width(), size().height());
  }
}

void GLViewportWidget::initializeGL()
{
  try
  {
    m_viewport.callMethod("", "setup", 1, &m_drawContext);
    m_drawing = m_drawing.callMethod("OGLInlineDrawing", "getInstance", 0, 0);
    setGridVisible(m_gridVisible, false);
    emit initComplete();
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "GLViewportWidget::initializeGL",
      "",
      e.getDesc_cstr());
  }
}

void GLViewportWidget::resizeGL(int width, int height)
{
  m_width = width;
  m_height = height;

  try
  {
    Context context = FabricApplicationStates::GetAppStates()->getContext();

    RTVal args[3] = {
      m_drawContext,
      RTVal::ConstructUInt32(context, (unsigned int)m_width),
      RTVal::ConstructUInt32(context, (unsigned int)m_height)
    };

    m_viewport.callMethod("", "resize", 3, args);
    m_resizedOnce = true;
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "GLViewportWidget::resizeGL",
      "",
      e.getDesc_cstr());
  }
}

void GLViewportWidget::paintGL()
{
  if(!m_resizedOnce)
  {
    QSize scale = size();
    resizeGL(scale.width(), scale.height());
  }

  computeFPS();

  try
  {
    RTVal args[2] = {
      RTVal::ConstructString(FabricApplicationStates::GetAppStates()->getContext(), "default"),
      m_drawContext
    };
    m_drawing.callMethod("", "drawViewport", 2, args);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "GLViewportWidget::paintGL",
      "",
      e.getDesc_cstr());
  }

  swapBuffers();
  emit redrawn();
}

void GLViewportWidget::resetRTVals( bool shouldUpdateGL )
{
  try
  {
    Context context = FabricApplicationStates::GetAppStates()->getContext();

    m_drawing = RTVal::Create(context, "OGLInlineDrawing", 0, 0);
    if(!m_drawing.isValid())
    {
      printf("[GLWidget] Error: Cannot construct OGLInlineDrawing RTVal (extension loaded?)\n");
      return;
    }
    m_drawing = m_drawing.callMethod("OGLInlineDrawing", "getNewInstance", 0, 0);

    m_viewport = RTVal::Create(context, "OGLStandaloneViewport", 0, 0);
    if(!m_viewport.isValid())
    {
      printf("[GLWidget] Error: Cannot construct OGLStandaloneViewport RTVal (extension loaded?)\n");
      return;
    }
    else
    {
      RTVal args[2] = {
        RTVal::ConstructString(context, "default"),
        m_viewport
      };

      m_drawing.callMethod("", "registerViewport", 2, args);

      // [pzion 20150909] No viewport overlay, at least for now
      // if(!m_hasCommercialLicense)
      //   m_viewport.callMethod("", "setupLicensingOverlay", 0, 0);
    }

    // Proto tool setup
    RTVal::Create( context, "WRenderEngineInlineDrawingSetup", 0, 0 );

    m_camera = m_viewport.maybeGetMember("camera");
    m_cameraManipulator = RTVal::Create(context, "CameraManipulator", 1, &m_camera);

    m_viewport.setMember("windowId", RTVal::ConstructUInt64(context, (uint64_t)this->winId()));

    setBackgroundColor(m_bgColor);

    m_drawContext = RTVal::Create(context, "DrawContext", 0, 0);
    if(!m_drawContext.isValid())
    {
      printf("[GLWidget] Error: Cannot construct DrawContext RTVal (extension loaded?)\n");
      return;
    }
    m_drawContext = m_drawContext.callMethod("DrawContext", "getInstance", 0, 0);
    m_drawContext.callMethod("", "resetAllViewportParams", 0, 0);

    // [pzion 20150728] Artificially push a meaningless movement
    // event to force camera manipulation code to compiler, preventing
    // annoying pauses
    QMouseEvent nullEvent(
      QMouseEvent::MouseMove,
      QCursor::pos(),
      Qt::NoButton,
      Qt::NoButton,
      Qt::NoModifier
      );

    manipulateCamera(
      &nullEvent,
      false // shouldUpdateGL
      );
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "GLViewportWidget::resetRTVals",
      "",
      e.getDesc_cstr());
  }

  setGridVisible( m_gridVisible, shouldUpdateGL );
  emit initComplete();
}

bool GLViewportWidget::manipulateCamera(
  QEvent *event,
  bool shouldUpdateGL
  )
{
  bool result = false;

  try
  {
    // Now we translate the Qt events to FabricEngine events..
    RTVal klevent = QtToKLEvent(event, FabricApplicationStates::GetAppStates()->getClient(), m_viewport, "Canvas");

    // And then pass the event to the camera manipulator for handling.
    m_cameraManipulator.callMethod("", "onEvent", 1, &klevent);
    result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "GLViewportWidget::manipulateCamera",
      "",
      e.getDesc_cstr());
  }

  if ( shouldUpdateGL )
    update();

  return result;
}

bool GLViewportWidget::isGridVisible()
{
  return m_gridVisible;
}

void GLViewportWidget::setGridVisible( bool gridVisible, bool updateView )
{
  m_gridVisible = gridVisible;

  QSettings* settings = FabricApplicationStates::GetAppStates()->getSettings();
  if(settings)
    settings->setValue("glviewport/gridVisible", m_gridVisible);

  if(!m_viewport.isValid())
    return;

  try
  {
    Context context = FabricApplicationStates::GetAppStates()->getContext();
    RTVal gridVisibleVal = RTVal::ConstructBoolean(context, m_gridVisible);
    m_viewport.callMethod("", "setGridVisible", 1, &gridVisibleVal);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "GLViewportWidget::setGridVisible",
      "",
      e.getDesc_cstr());
  }

  if( updateView )
    update();
}

void GLViewportWidget::resetCamera()
{
  if(!m_viewport.isValid())
    return;

  try
  {
    Context context = FabricApplicationStates::GetAppStates()->getContext();
    
    RTVal position[3];
    position[0] = RTVal::ConstructFloat32(context, 30.0f);
    position[1] = RTVal::ConstructFloat32(context, 20.0f);
    position[2] = RTVal::ConstructFloat32(context, 40.0f);
    
    RTVal target[3];
    target[0] = RTVal::ConstructFloat32(context, 0.0f);
    target[1] = RTVal::ConstructFloat32(context, 0.0f);
    target[2] = RTVal::ConstructFloat32(context, 0.0f);
    
    RTVal args[2] = {
      RTVal::Construct(context, "Vec3", 3, position),
      RTVal::Construct(context, "Vec3", 3, target)
    };

    m_camera.callMethod("", "setFromPositionAndTarget", 2, args);
    m_cameraManipulator = RTVal::Create(context, "CameraManipulator", 1, &m_camera);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "GLViewportWidget::resetCamera",
      "",
      e.getDesc_cstr());
  }

  update();
}

