
#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FTL/Config.h>

#include <QStyleOption>
#include <QPainter>
#include <QDebug>

using namespace FabricUI::FCurveEditor;

inline QPointF min( const QPointF& a, const QPointF& b )
{
  return QPointF(
    std::min( a.x(), b.x() ),
    std::min( a.y(), b.y() )
  );
}

inline QPointF max( const QPointF& a, const QPointF& b )
{
  return QPointF(
    std::max( a.x(), b.x() ),
    std::max( a.y(), b.y() )
  );
}

class FCurveEditor::FCurveShape : public QGraphicsItem
{
  const FCurveEditor* m_parent;
  typedef QGraphicsItem Parent;

public:
  FCurveShape( const FCurveEditor* parent )
    : m_parent( parent )
  {
  }

  QRectF boundingRect() const FTL_OVERRIDE
  {
    // TODO : cache and update when handles are moved
    QPointF topLeft = QPointF( 1, 1 ) * std::numeric_limits<qreal>::max();
    QPointF botRight = QPointF( 1, 1 ) * std::numeric_limits<qreal>::min();
    if( m_parent->m_curve != NULL )
    {
      size_t hc = m_parent->m_curve->getHandleCount();
      for( size_t i = 0; i < hc; i++ )
      {
        Handle h = m_parent->m_curve->getHandle( i );
        topLeft = min( topLeft, h.pos );
        botRight = max( botRight, h.pos );
        topLeft = min( topLeft, h.pos + h.tanIn );
        botRight = max( botRight, h.pos + h.tanIn );
        topLeft = min( topLeft, h.pos + h.tanOut );
        botRight = max( botRight, h.pos + h.tanOut );
        // TODO : tangents correctly used ?
      }
    }
    QRectF dst( topLeft, botRight );
    return dst;
  }

  void paint(
    QPainter * painter,
    const QStyleOptionGraphicsItem * option,
    QWidget * widget = Q_NULLPTR
  ) FTL_OVERRIDE
  {
    if( m_parent->m_curve != NULL )
    {
      const QRectF er = painter->transform().inverted()
        .mapRect( QRectF( widget->geometry() ) ).intersected( this->boundingRect() );
      QPen pen; pen.setCosmetic( true );
      pen.setWidthF( 2 );
      pen.setColor( QColor( 0, 255, 255 ) );
      painter->setPen( pen );
      const size_t n = widget->width() / 8;
      for( size_t i = 0; i < n; i++ )
      {
        qreal x1 = er.left() + ( i * er.width() ) / n;
        qreal x2 = er.left() + ( ( i + 1 ) * er.width() ) / n;
        painter->drawLine(
          QPointF( x1, m_parent->m_curve->evaluate( x1 ) ),
          QPointF( x2, m_parent->m_curve->evaluate( x2 ) )
        );
      }
    }
  }

};

FCurveEditor::FCurveEditor()
  : m_curve( NULL )
  , m_curveShape( new FCurveShape( this ) )
{
  m_curveShape->setParentItem( this );
}

void FCurveEditor::setCurve( AbstractFCurveModel* curve )
{
  m_curve = curve;
}
