
#include <FabricUI/FCurveEditor/Ruler.h>

#include <cmath>
#include <QPainter>
#include <QDebug>

using namespace FabricUI::FCurveEditor;

Ruler::Ruler( Qt::Orientation o )
  : m_orientation( o )
  , m_majorGradsPixelSpacing( 800.0f )
  , m_majToMinGradsRatio( 8 )
  , m_logScale( 2 )
  , m_penColor( QColor( 128, 128, 128 ) )
  , m_majorPenWidth( 2 )
  , m_minorPenWidth( 1 )
{
  this->setObjectName( "Ruler" );
}

void Ruler::setRange( qreal start, qreal end )
{
  m_start = start;
  m_end = end;
  this->update();
}

inline qreal Map(
  qreal v, // value to be mapped
  qreal minB, qreal maxB, // original range
  qreal minA, qreal maxA // new range
)
{
  return minA + ( v - minB ) * ( maxA - minA ) / ( maxB - minB );
}

inline qreal Map(
  qreal v, // value to be mapped
  Qt::Orientation o,
  qreal min, qreal max,
  QRect r
)
{
  return o == Qt::Vertical ?
    Map( v, min, max, r.top(), r.bottom() ) :
    Map( v, min, max, r.left(), r.right() )
  ;
}

inline void DrawLine( QPainter* p, qreal pos, Qt::Orientation o, qreal endRPos, const QRect& r )
{
  if( o == Qt::Horizontal )
    p->drawLine( QPointF( pos, r.top() ), QPointF( pos, ( 1 - endRPos ) * r.top() + endRPos * r.bottom() ) );
  else
    p->drawLine( QPointF( endRPos * r.left() + ( 1 - endRPos ) * r.right(), pos ), QPointF( r.right(), pos ) );
}

inline void DrawText( QPainter* p, qreal pos, Qt::Orientation o, qreal endRPos, const QRect& r, const QString& txt )
{
  if( o == Qt::Horizontal )
    p->drawText( QPointF( pos, endRPos * r.bottom() + ( 1 - endRPos ) * r.top() ), txt );
  else
  {
    p->rotate( 90 );
    p->drawText( QPointF( pos, -( endRPos * r.left() + ( 1 - endRPos ) * r.right() ) ), txt );
    p->rotate( -90 );
  }
}

void Ruler::paintEvent( QPaintEvent * e )
{
  Parent::paintEvent( e );

  if( m_start == m_end )
    return;

  const QRect r = this->contentsRect();
  const size_t bigSide = ( m_orientation == Qt::Vertical ? r.height() : r.width() );
  qreal bigFactor = std::pow( m_logScale,
    std::ceil( -std::log( std::abs( m_end - m_start ) * m_majorGradsPixelSpacing / bigSide ) / std::log( m_logScale ) ) ) * 4;
  qreal smallFactor = bigFactor * m_majToMinGradsRatio;
  {
    QPainter p( this );
    QPen pen;
    pen.setColor( m_penColor );
    pen.setWidthF( m_majorPenWidth );
    p.setPen( pen );
    qreal minV = std::min( m_start, m_end );
    qreal maxV = std::max( m_start, m_end );
    for( qreal i = std::floor( minV * bigFactor ); i < bigFactor * maxV; i++ )
    {
      qreal xs = i / bigFactor;
      qreal xw = Map( xs, m_orientation, m_start, m_end, r );
      DrawLine( &p, xw, m_orientation, 0.5, r );
      DrawText( &p, xw + 5, m_orientation, 0.9f, r, QString::number( xs ) );
    }
    pen.setWidthF( m_minorPenWidth );
    p.setPen( pen );
    for( qreal i = std::floor( minV * smallFactor ); i < smallFactor * maxV; i++ )
    {
      qreal xs = i / smallFactor;
      qreal xw = Map( xs, m_orientation, m_start, m_end, r );
      DrawLine( &p, xw, m_orientation, 0.3f, r );
    }
  }
}
