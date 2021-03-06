// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/PinCircle.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>

#include <QDebug>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

using namespace FabricUI::GraphView;

PinCircle::PinCircle(
  ConnectionTarget *parent,
  PortType portType,
  QColor color,
  bool interactiveConnectionsAllowed,
  bool invisible
  )
  : QGraphicsWidget( parent )
  , m_invisible( invisible )
{
  m_target = parent;
  m_portType = portType;
  m_radius = m_target->graph()->config().pinRadius;
  m_highlighted = false;
  m_interactiveConnectionsAllowed = interactiveConnectionsAllowed;
  m_shouldBeVisible = true;

  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  setPreferredSize(diameter(), diameter());
  setWindowFrameMargins(0, 0, 0, 0);

  m_defaultPen = m_target->graph()->config().pinDefaultPen;
  m_hoverPen = m_target->graph()->config().pinHoverPen;
  setAcceptHoverEvents(true);

  m_ellipse = new QGraphicsEllipseItem(this);
  m_ellipse->setPos(radius(), radius());
  m_ellipse->setRect(-radius(), -radius(), diameter(), diameter());

  if (!m_target->graph()->config().pinDisplayAsFullCircle)
  {
    if(portType == PortType_Input)
    {
      m_ellipse->setStartAngle(270 * 16);
      m_ellipse->setSpanAngle(180 * 16);
    }
    else
    {
      m_ellipse->setStartAngle(90 * 16);
      m_ellipse->setSpanAngle(180 * 16);
    }
  }

  m_ellipse->setVisible( !m_invisible );

  setColor(color);
}

ConnectionTarget * PinCircle::target()
{
  return m_target;
}

const ConnectionTarget * PinCircle::target() const
{
  return m_target;
}

QColor PinCircle::color() const
{
  return m_color;
}

void PinCircle::setColor(QColor color)
{
  m_color = color;

  if(m_target->graph()->config().pinUsesColorForPen)
  {
    m_defaultPen.setColor(m_color.darker());
    m_hoverPen.setColor(m_color);
  }

  if(highlighted())
  {
    m_ellipse->setPen(m_hoverPen);
    m_ellipse->setBrush(QBrush(m_color.lighter(), Qt::SolidPattern));
  }
  else
  {
    m_ellipse->setPen(m_defaultPen);
    m_ellipse->setBrush(QBrush(m_color, Qt::SolidPattern));
  }
}

bool PinCircle::highlighted() const
{
  return m_highlighted;
}

void PinCircle::setHighlighted(bool state)
{
  if(m_highlighted == state)
    return;
  m_highlighted = state;
  setColor(m_color);
}

QPen PinCircle::defaultPen() const
{
  return m_defaultPen;
}

QPen PinCircle::hoverPen() const
{
  return m_hoverPen;
}

float PinCircle::radius() const
{
  return m_radius;
}

float PinCircle::diameter() const
{
  return radius() * 2.0f;
}

QPointF PinCircle::centerInSceneCoords() const
{
  return mapToScene( radius(), radius() );
}

PortType PinCircle::portType() const
{
  return m_portType;
}

bool PinCircle::isInputPortType() const
{
  return portType() == PortType_Input;
}

bool PinCircle::isOutputPortType() const
{
  return portType() == PortType_Output;
}

void PinCircle::onHoverEnter()
{
  m_ellipse->setVisible( !m_invisible );
  setHighlighted( true );

  if(target()->targetType() != TargetType_NodeHeader)
    target()->setHighlighted(true);
}

void PinCircle::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  onHoverEnter();

  QGraphicsItem *gi = parentItem();
  while ( gi )
  {
    if ( gi->type() == QGraphicsItemType_Node )
      break;
    gi = gi->parentItem();
  }
  if ( gi )
    static_cast<Node *>( gi )->updateHighlightingFromChild( this, event->pos() );

  QGraphicsWidget::hoverEnterEvent(event);
}

void PinCircle::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  QGraphicsItem *gi = parentItem();
  while ( gi )
  {
    if ( gi->type() == QGraphicsItemType_Node )
      break;
    gi = gi->parentItem();
  }
  if ( gi )
    static_cast<Node *>( gi )->updateHighlightingFromChild( this, event->pos() );

  QGraphicsWidget::hoverMoveEvent(event);
}

void PinCircle::onHoverLeave()
{
  m_ellipse->setVisible( !m_invisible && m_shouldBeVisible );
  setHighlighted( false );

  if(target()->targetType() != TargetType_NodeHeader)
    target()->setHighlighted(false);
}

void PinCircle::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  onHoverLeave();

  QGraphicsItem *gi = parentItem();
  while ( gi )
  {
    if ( gi->type() == QGraphicsItemType_Node )
      break;
    gi = gi->parentItem();
  }
  if ( gi )
    static_cast<Node *>( gi )->updateHighlightingFromChild( this, event->pos() );

  QGraphicsWidget::hoverLeaveEvent(event);
}

void PinCircle::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if( MainPanel::filterMousePressEvent( event ) )
    return event->ignore();

  // with context menus we'll receive more
  // events, even for clicks outside of our 
  // bounding rect
  if (
    (
      target()->targetType() == TargetType_NodeHeader ||
      target()->targetType() == TargetType_InstBlockHeader
    )
    &&  !rect().contains( event->pos() ) )
  {
    QGraphicsWidget::mousePressEvent( event );
    return;
  }

  if(event->button() == Qt::LeftButton && m_interactiveConnectionsAllowed)
  {
    if(target()->targetType() != TargetType_NodeHeader)
      target()->setHighlighted(false);

    QPointF scenePos = mapToScene(QPointF(m_radius * 0.5, m_radius * 0.5));

    // once the mousegrabber is destroyed
    // it will call end interaction
    target()->graph()->controller()->beginInteraction();

    if(portType() == PortType_Output)
      target()->graph()->constructMouseGrabber(scenePos, target(), PortType_Input);
    else
      target()->graph()->constructMouseGrabber(scenePos, target(), PortType_Output);
  }
  else if(event->button() == Qt::RightButton)
  {
    QMenu * menu = NULL;
    if(target()->targetType() == TargetType_Pin)
      menu = target()->graph()->getPinContextMenu( ( Pin* )this->target() );
    else if(target()->targetType() == TargetType_Port)
      menu = target()->graph()->getPortContextMenu( ( Port* )this->target() );
    else if(target()->targetType() == TargetType_FixedPort)
      menu = target()->graph()->getFixedPortContextMenu((FixedPort*)this->target());

    if(menu)
    {
      menu->exec(QCursor::pos());
      menu->setParent( NULL );
      menu->deleteLater();
    }
  }
  else
    QGraphicsWidget::mousePressEvent(event);
}

void PinCircle::setDaisyChainCircleVisible(bool state)
{
  m_ellipse->setVisible( !m_invisible && state );
  m_shouldBeVisible = state;
}
