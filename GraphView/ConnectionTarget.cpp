// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ConnectionTarget.h"
#include "Graph.h"
#include "PinCircle.h"
#include "InstBlockPort.h"
#include <FabricUI/GraphView/MainPanel.h>

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

using namespace FabricUI::GraphView;

ConnectionTarget::ConnectionTarget(QGraphicsWidget * parent)
  : QGraphicsWidget(parent) 
  , m_lastPinCircle( NULL )
#if defined(FTL_BUILD_DEBUG)
  , m_deleted( false )
#endif
{
  setAcceptHoverEvents(true);
}

ConnectionTarget::~ConnectionTarget()
{
#if defined(FTL_BUILD_DEBUG)
  m_deleted = true;
#endif
}

bool ConnectionTarget::isConnected() const
{
  return graph()->isConnected(this);
}

bool ConnectionTarget::isConnectedAsSource() const
{
  return graph()->isConnectedAsSource(this);
}

bool ConnectionTarget::isConnectedAsTarget() const
{
  return graph()->isConnectedAsTarget(this);
}

bool ConnectionTarget::isDragging() {
  return targetType() == TargetType_MouseGrabber;
}

void ConnectionTarget::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  PinCircle * circle = findPinCircle(event->pos());
  if(circle)
  {
    circle->onHoverEnter();
    m_lastPinCircle = circle;
  }

  QGraphicsWidget::hoverEnterEvent(event);
}

void ConnectionTarget::hoverMoveEvent( QGraphicsSceneHoverEvent * event )
{
  if(m_lastPinCircle)
    m_lastPinCircle->onHoverLeave();
  PinCircle * circle = findPinCircle(event->pos());
  if(circle)
  {
    circle->onHoverEnter();
    m_lastPinCircle = circle;
  }
  QGraphicsWidget::hoverMoveEvent(event);
}

void ConnectionTarget::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  if(m_lastPinCircle)
    m_lastPinCircle->onHoverLeave();
  QGraphicsWidget::hoverLeaveEvent(event);
}

void ConnectionTarget::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if( MainPanel::filterMousePressEvent( event ) )
    return event->ignore();

  if( event->button() == Qt::LeftButton )
  {
    PinCircle * circle = findPinCircle(event->pos());
    if(circle)
    {
      circle->mousePressEvent( event );
      return;
    }
  }

  QGraphicsWidget::mousePressEvent(event);
}

bool ConnectionTarget::selected() const
{
  return false;
}
