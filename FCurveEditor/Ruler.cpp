
#include <FabricUI/FCurveEditor/Ruler.h>

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

void Ruler::paintEvent( QPaintEvent * e )
{
  Parent::paintEvent( e );

  if( m_min == m_max )
    return;

  float bigFactor = std::powf( 2, std::ceil( -std::log2( m_max - m_min ) ) ) * 4;
  float smallFactor = bigFactor * 8;

  // TODO : vertical
  if( m_orientation == Qt::Horizontal )
  {

    const QRect r = this->contentsRect();
    QPainter p( this );
    QPen pen;
    pen.setColor( QColor( 128, 128, 128 ) );
    pen.setWidthF( 4 );
    p.setPen( pen );
    for( float i = std::floor( m_min * bigFactor ); i < bigFactor * m_max; i++ )
    {
      float xs = i / bigFactor;
      float xw = Map( xs, m_orientation, m_min, m_max, r );
      float middle = ( r.top() + r.bottom() ) / 2;
      p.drawLine( QPointF( xw, r.top() ), QPointF( xw, middle ) );
      p.drawText( QPointF( xw, 0.9 * r.bottom() + 0.1 * r.top() ), QString::number( xs ) );
    }
    pen.setWidthF( 1 );
    p.setPen( pen );
    for( float i = std::floor( m_min * smallFactor ); i < smallFactor * m_max; i++ )
    {
      float xs = i / smallFactor;
      float xw = Map( xs, m_orientation, m_min, m_max, r );
      float smallRatio = 0.3;
      float end = ( 1 - smallRatio ) * r.top() + smallRatio * r.bottom();
      p.drawLine( QPointF( xw, r.top() ), QPointF( xw, end ) );
    }
  }
}
