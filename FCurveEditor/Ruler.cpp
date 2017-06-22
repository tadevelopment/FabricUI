
#include <FabricUI/FCurveEditor/Ruler.h>

#include <QPainter>

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

  // TODO : vertical
  if( m_orientation == Qt::Horizontal )
  {
    const QRect r = this->contentsRect();
    QPainter p( this );
    p.setPen( QColor( 128, 128, 128 ) );
    for( float i = 0; i < 1; i += 0.1 )
    {
      float x = Map( i, m_orientation, m_min, m_max, r );
      p.drawLine( QPointF( x, r.top() ), QPointF( x, r.bottom() ) );
    }
  }
}
