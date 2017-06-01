/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */
 
#include "ViewportWidget.h"
#include "ViewportEventFilter.h"

using namespace FabricUI;
using namespace Viewports;

ViewportEventFilter::ViewportEventFilter(
  ViewportWidget *viewport)
 : QObject()
 , m_viewport(viewport) 
{
}

bool ViewportEventFilter::eventFilter(
  QObject *object, 
  QEvent *event)
{
  // QEvent::ShortcutOverride is always called first.
  // Use it instead of QEvent::KeyPress so we catch
  // all the key-strockes.
  return (event->type() != QEvent::KeyPress)  
    ? m_viewport->onEvent(event)
    : false;
}
