//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/FCurveEditor/RuledGraphicsView.h>
#include <QGraphicsView>
#include <QLayout>

#include <qevent.h>
#include <QDebug>
#include <QTimer>

using namespace FabricUI::FCurveEditor;

class RuledGraphicsView::GraphicsView : public QGraphicsView
{
public:
  GraphicsView()
  {
    this->setDragMode( QGraphicsView::ScrollHandDrag );
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  }
protected:
  void wheelEvent( QWheelEvent * e ) FTL_OVERRIDE { return e->ignore(); } // HACK ?
  void drawBackground( QPainter *, const QRectF & ) FTL_OVERRIDE;
};

QGraphicsView* RuledGraphicsView::view() { return m_view; }

class RuledGraphicsView::Ruler : public QFrame
{
  RuledGraphicsView* m_parent;
  bool m_isVertical;

public:
  Ruler( RuledGraphicsView* parent, bool isVertical )
    : m_parent( parent )
    , m_isVertical( isVertical )
  {}

protected:
  void wheelEvent( QWheelEvent * e ) FTL_OVERRIDE
  {
    m_parent->wheelEvent(
      m_isVertical ? 0 : e->delta(),
      m_isVertical ? e->delta() : 0
    );
    e->accept();
  }
  void enterEvent( QEvent *event ) FTL_OVERRIDE
  {
    this->setCursor( m_isVertical ? Qt::SizeVerCursor : Qt::SizeHorCursor );
  }
  void leaveEvent( QEvent *event ) FTL_OVERRIDE { this->unsetCursor(); }
};

RuledGraphicsView::RuledGraphicsView()
  : m_view( new GraphicsView() )
  , m_scrollSpeed( 1 / 800.0f )
  , m_smoothZoom( true )
  // HACK : update m_targetScale when methods such as fitInView() are called
  , m_targetScale( QPointF( 1E2, 1E2 ) )
  , m_timer( new QTimer( this ) )
{
  QGridLayout* lay = new QGridLayout();
  lay->setSpacing( 0 ); lay->setMargin( 0 );

  this->setStyleSheet( "background-color: #333; border: #000;" );

  lay->addWidget( m_view, 0, 1 );

  m_vRuler = new Ruler( this, true );
  m_vRuler->setFixedWidth( 50 );
  m_vRuler->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ) );
  lay->addWidget( m_vRuler, 0, 0 );

  m_hRuler = new Ruler( this, false );
  m_hRuler->setFixedHeight( 50 );
  m_hRuler->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  lay->addWidget( m_hRuler, 1, 1 );

  this->setLayout( lay );

  connect( m_timer, &QTimer::timeout, this, &RuledGraphicsView::tick );
  m_timer->setInterval( 16 );
  if( m_smoothZoom )
    m_timer->start();
}

void RuledGraphicsView::wheelEvent( QWheelEvent * e )
{
  this->wheelEvent( e->delta(), e->delta() );
}

void RuledGraphicsView::wheelEvent( int xDelta, int yDelta )
{
  float sX = ( 1 + xDelta * m_scrollSpeed );
  float sY = ( 1 + yDelta * m_scrollSpeed );
  if( m_smoothZoom )
  {
    m_targetScale.setX( m_targetScale.x() * sX );
    m_targetScale.setY( m_targetScale.y() * sY );
    m_timer->start();
  }
  else
    m_view->scale( sX, sY );
}

void RuledGraphicsView::tick()
{
  QPointF currentScale = QPointF( m_view->matrix().m11(), m_view->matrix().m22() );
  if( abs(
    std::log( QPointF::dotProduct( currentScale, currentScale ) ) -
    std::log( QPointF::dotProduct( m_targetScale, m_targetScale ) )
  ) > 0.1 )
  {
    const float ratio = 0.2;
    currentScale.setX( ( 1 - ratio ) * currentScale.x() + ratio * m_targetScale.x() );
    currentScale.setY( ( 1 - ratio ) * currentScale.y() + ratio * m_targetScale.y() );
    m_view->resetTransform();
    m_view->scale( currentScale.x(), currentScale.y() );
  }
  else
    m_timer->stop();
}

void RuledGraphicsView::GraphicsView::drawBackground( QPainter * p, const QRectF & r )
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
