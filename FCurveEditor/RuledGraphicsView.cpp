//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/FCurveEditor/RuledGraphicsView.h>

#include <qevent.h>
#include <QDebug>
#include <QTimer>

using namespace FabricUI::FCurveEditor;

RuledGraphicsView::RuledGraphicsView()
  : m_scrollSpeed( 1 / 800.0f )
  , m_smoothZoom( true )
  // HACK : update m_targetScale when methods such as fitInView() are called
  , m_targetScale( QPointF( 1E2, 1E2 ) )
  , m_timer( new QTimer( this ) )
{
  this->setDragMode( QGraphicsView::ScrollHandDrag );
  this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  connect( m_timer, &QTimer::timeout, this, &RuledGraphicsView::tick );
  m_timer->setInterval( 16 );
  if( m_smoothZoom )
    m_timer->start();
}

void RuledGraphicsView::wheelEvent( QWheelEvent * e )
{
  float s = ( 1 + e->delta() * m_scrollSpeed );
  if( m_smoothZoom )
  {
    m_targetScale.setX( m_targetScale.x() * s );
    m_targetScale.setY( m_targetScale.y() * s );
    m_timer->start();
  }
  else
    this->scale( s, s );
}

void RuledGraphicsView::tick()
{
  QPointF currentScale = QPointF( this->matrix().m11(), this->matrix().m22() );
  if( abs(
    std::log( QPointF::dotProduct( currentScale, currentScale ) ) -
    std::log( QPointF::dotProduct( m_targetScale, m_targetScale ) )
  ) > 0.1 )
  {
    const float ratio = 0.2;
    currentScale.setX( ( 1 - ratio ) * currentScale.x() + ratio * m_targetScale.x() );
    currentScale.setY( ( 1 - ratio ) * currentScale.y() + ratio * m_targetScale.y() );
    this->resetTransform();
    this->scale( currentScale.x(), currentScale.y() );
  }
  else
    m_timer->stop();
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
