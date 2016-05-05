//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "BlockRectangle.h"
#include "Node.h"
#include "Graph.h"

#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QStyleOptionGraphicsItem>

#ifdef FABRICUI_TIMERS
  #include <Util/Timer.h>
#endif

using namespace FabricUI::GraphView;

BlockRectangle::BlockRectangle( Node *node )
  : QGraphicsWidget( node )
  , m_node( node )
{
}

void BlockRectangle::paint(
  QPainter *painter,
  QStyleOptionGraphicsItem const *option,
  QWidget *widget
  )
{
#ifdef FABRICUI_TIMERS
  Util::TimerPtr timer = Util::Timer::getTimer("FabricUI::BlockRectangle");
  timer->resume();
#endif

  QPen standardPen = m_node->m_defaultPen;
  if ( m_node->selected() )
    standardPen = m_node->m_selectedPen;
  // if(m_node->hasError())
  //   standardPen = m_node->m_errorPen;

  QRectF rect = windowFrameRect();

  float nodeWidthReduction = m_node->graph()->config().nodeWidthReduction * 0.5;
  rect.adjust(nodeWidthReduction, standardPen.width() * 0.5f, -nodeWidthReduction, -standardPen.width() * 0.5f);

  QLinearGradient gradient(0.5, 0.0, 0.5, 1.0);
  gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
  gradient.setColorAt(0.0, m_node->m_colorA);
  gradient.setColorAt(1.0, m_node->m_colorB);

  painter->setBrush(gradient);

  // painter->setRenderHint(QPainter::Antialiasing,true);
  // painter->setRenderHint(QPainter::HighQualityAntialiasing,true);

  qreal headerHeight = m_node->m_header->size().height();

  static qreal const pipWidth = 12;
  static qreal const pitWidth = 8;
  static qreal const height = 3;

  QPainterPath path;
  path.moveTo( rect.topLeft() );
  qreal x = 0;
  while ( rect.right() - x >= pipWidth + pitWidth + pipWidth )
  {
    x += pipWidth;
    path.lineTo( rect.left() + x, rect.top() );
    path.lineTo( rect.left() + x, rect.top() + height );
    x += pitWidth;
    path.lineTo( rect.left() + x, rect.top() + height );
    path.lineTo( rect.left() + x, rect.top() );
  }
  path.lineTo( rect.topRight() );
  path.lineTo( rect.bottomRight() );
  path.lineTo( rect.bottomLeft() );
  path.lineTo( rect.topLeft() );

  // fill everything
  painter->fillPath(path,painter->brush());     

  // draw the label
  QRectF labelRect( rect.left(), rect.top(), rect.width(), headerHeight );
  painter->setClipPath(path);
  painter->setClipRect(labelRect, Qt::IntersectClip);
  painter->setBrush(m_node->m_titleColor);
  painter->fillPath(path,painter->brush());     

  // remove the clipping
  painter->setClipRect(rect.adjusted(-2, -2, 2, 2));

  // draw the header lines
  // FE-4157
  // QPen coloredPen = standardPen;
  // coloredPen.setColor(m_node->color());
  // painter->setPen(coloredPen);
  // painter->drawLine(labelRect.bottomLeft() - QPointF(0, standardPen.width()), labelRect.bottomRight() - QPointF(0, standardPen.width()));
  painter->setPen(standardPen);
  painter->drawLine(
    labelRect.bottomLeft(),
    labelRect.bottomRight()
    );

  // draw the outline
  painter->strokePath(path, standardPen);

#ifdef FABRICUI_TIMERS
  timer->pause();
#endif

  painter->setClipping(false);
  QGraphicsWidget::paint(painter, option, widget);

}
