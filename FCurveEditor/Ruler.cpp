
#include <FabricUI/FCurveEditor/Ruler.h>

#include <cmath>
#include <QPainter>
#include <QDebug>

using namespace FabricUI::FCurveEditor;

Ruler::Ruler( Qt::Orientation o )
  : m_orientation( o )
{

}

void Ruler::setRange( float min, float max )
{
  m_min = min;
  m_max = max;
  this->update();
}

inline float Map(
  float v, // value to be mapped
  float minB, float maxB, // original range
  float minA, float maxA // new range
)
{
  return minA + ( v - minB ) * ( maxA - minA ) / ( maxB - minB );
}

inline float Map(
  float v, // value to be mapped
  Qt::Orientation o,
  float min, float max,
  QRect r
)
{
  return o == Qt::Vertical ?
    Map( v, min, max, r.top(), r.bottom() ) :
    Map( v, min, max, r.left(), r.right() )
  ;
}

inline void DrawLine( QPainter* p, float pos, Qt::Orientation o, float endRPos, const QRect& r )
{
  if( o == Qt::Horizontal )
    p->drawLine( QPointF( pos, r.top() ), QPointF( pos, ( 1 - endRPos ) * r.top() + endRPos * r.bottom() ) );
  else
    p->drawLine( QPointF( endRPos * r.left() + ( 1 - endRPos ) * r.right(), pos ), QPointF( r.right(), pos ) );
}

inline void DrawText( QPainter* p, float pos, Qt::Orientation o, float endRPos, const QRect& r, const QString& txt )
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

  if( m_min == m_max )
    return;

  float bigFactor = std::pow( 2, std::ceil( -std::log( m_max - m_min ) / std::log( 2 ) ) ) * 4;
  float smallFactor = bigFactor * 8;
  {
    const QRect r = this->contentsRect();
    QPainter p( this );
    QPen pen;
    pen.setColor( QColor( 128, 128, 128 ) );
    pen.setWidthF( 2 );
    p.setPen( pen );
    size_t smallSide = ( m_orientation == Qt::Vertical ? r.width() : r.height() );
    for( float i = std::floor( m_min * bigFactor ); i < bigFactor * m_max; i++ )
    {
      float xs = i / bigFactor;
      float xw = Map( xs, m_orientation, m_min, m_max, r );
      DrawLine( &p, xw, m_orientation, 0.5, r );
      DrawText( &p, xw + smallSide / 4, m_orientation, 0.9f, r, QString::number( xs ) );
    }
    pen.setWidthF( 1 );
    p.setPen( pen );
    for( float i = std::floor( m_min * smallFactor ); i < smallFactor * m_max; i++ )
    {
      float xs = i / smallFactor;
      float xw = Map( xs, m_orientation, m_min, m_max, r );
      DrawLine( &p, xw, m_orientation, 0.3f, r );
    }
  }
}
