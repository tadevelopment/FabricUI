//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/FCurveEditor/RuledGraphicsView.h>

#include <qevent.h>
#include <QDebug>

using namespace FabricUI::FCurveEditor;

RuledGraphicsView::RuledGraphicsView()
{
  this->setDragMode( QGraphicsView::ScrollHandDrag );
  this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
}

void RuledGraphicsView::wheelEvent( QWheelEvent * e )
{
  float s = ( 1 + e->delta() * m_scrollSpeed );
  this->scale( s, s );
}

void RuledGraphicsView::drawBackground( QPainter * p, const QRectF & r )
{
  // Background
  p->fillRect( r, QColor( 64, 64, 64 ) );

  QRect wr = this->viewport()->geometry(); // widget viewRect
  QRectF sr = this->mapToScene( wr ).boundingRect(); // scene viewRect

  // Grid X
  {
    float minFactor = std::pow( 2, std::floor( std::log2f( 8.0f / sr.width() ) ) );
    float maxFactor = 150.0f / sr.width();
    for( float factor = minFactor; factor < maxFactor; factor *= 2 )
    {
      QPen pen; pen.setWidthF( 1E-2 / factor );
      p->setPen( pen );
      for( float i = std::ceil( factor * sr.left() ); i < factor * sr.right(); i++ )
        p->drawLine( QPointF( i / factor, sr.top() ), QPointF( i / factor, sr.bottom() ) );
    }
  }

  // Grid Y
  float yRatio = float( wr.height() ) / wr.width();
  {
    float minFactor = std::pow( 2, std::floor( std::log2f( 8.0f / sr.height() * yRatio ) ) );
    float maxFactor = 150.0f / sr.height() * yRatio;
    for( float factor = minFactor; factor < maxFactor; factor *= 2 )
    {
      QPen pen; pen.setWidthF( 1E-2 / factor );
      p->setPen( pen );
      for( float i = std::ceil( factor * sr.top() ); i < factor * sr.bottom(); i++ )
        p->drawLine( QPointF( sr.left(), i / factor ), QPointF( sr.right(), i / factor ) );
    }
  }
}
