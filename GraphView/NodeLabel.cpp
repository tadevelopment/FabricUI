// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/NodeLabel.h>
#include <FabricUI/GraphView/NodeHeader.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/MainPanel.h>

using namespace FabricUI::GraphView;

NodeLabel::NodeLabel(
  QGraphicsWidget * parent,
  Node* node,
  QString const &text,
  QColor color,
  QColor highlightColor,
  QFont font
  )
  : TextContainer(
    parent,
    text,
    color,
    highlightColor,
    font
    ),
  m_node(node)
{
}

void NodeLabel::displayedTextChanged()
{
  TextContainer::displayedTextChanged();
  m_node->adjustSize();
}

void NodeLabel::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if( MainPanel::filterMousePressEvent( event ) )
    return event->ignore();

  if( event->button() == Qt::LeftButton )
    m_node->onMousePress( event ); // we get the focus here, by accepting the event
  else
    TextContainer::mousePressEvent( event );
}

void NodeLabel::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  m_node->onMouseMove( event );
}

void NodeLabel::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  m_node->onMouseRelease( event );
}

void NodeLabel::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  m_node->onMouseDoubleClicked( event );
  TextContainer::mouseDoubleClickEvent(event);
}

