/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include <FabricCore.h>
#include "ViewportWidget.h"
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Viewports;
using namespace FabricCore;
using namespace Application;

void ViewportWidget::init() 
{  
  m_eventFilter = new ViewportEventFilter(this);
  installEventFilter(m_eventFilter);

  //setFocusPolicy(Qt::StrongFocus);
 
  m_fps = 0.0;
  for(int i=0;i<16;i++) 
    m_fpsStack[i] = 0.0;
}

ViewportWidget::ViewportWidget(
  QGLContext *qglContext, 
  QWidget *parent, 
  QGLWidget *share)
  : QGLWidget(qglContext, parent, share)
{ 
  init();
}

ViewportWidget::ViewportWidget(
  QGLFormat format, 
  QWidget *parent)
  : QGLWidget(format, parent)
{	
  init();
  setAutoBufferSwap(false);
}
 
ViewportWidget::~ViewportWidget() 
{
  delete(m_eventFilter);
}

bool ViewportWidget::isManipulationActive() 
{ 
  return false; 
}

void ViewportWidget::setManipulationActive(
  bool state) 
{
}

double ViewportWidget::fps() 
{ 
  return m_fps; 
}

void ViewportWidget::clear() 
{
}

void ViewportWidget::enterEvent(
  QEvent * event) 
{
  setFocus();
}

void ViewportWidget::leaveEvent(
  QEvent * event) 
{
  clearFocus();
}

bool ViewportWidget::onEvent(
  QEvent *event) 
{
  return false;
}

void ViewportWidget::redraw() 
{ 
  update(); 
} 

void ViewportWidget::toggleManipulation() 
{ 
  setManipulationActive(!isManipulationActive()); 
}

void ViewportWidget::computeFPS() 
{   
  // compute the fps
  double ms = m_fpsTimer.elapsed();
  m_fps = (ms == 0.0) ? 0.0 : 1000.0 / ms;
  double averageFps = 0.0;
  for(int i=0;i<15;i++) {
    m_fpsStack[i+1] = m_fpsStack[i];
    averageFps += m_fpsStack[i];
  }
  m_fpsStack[0] = m_fps;
  averageFps += m_fps;
  averageFps /= 16.0;
  m_fps = averageFps;
  m_fpsTimer.start();
}
