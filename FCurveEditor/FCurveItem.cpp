
#include <FabricUI/FCurveEditor/FCurveItem.h>
#include <FTL/Config.h>
#include <FabricUI/Util/QtSignalsSlots.h>

#include <QStyleOption>
#include <QPainter>
#include <QGraphicsSceneEvent>

#include <cmath>
#include <limits>

#include <QDebug>
#include <assert.h>

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

inline qreal len2( const QPointF& v ) { return v.x() * v.x() + v.y() * v.y(); }

class FCurveItem::FCurveShape : public QGraphicsItem
{
  const FCurveItem* m_parent;
  typedef QGraphicsItem Parent;
  mutable QRectF m_boundingRect;
  mutable bool m_boundingRectDirty;

  void updateBoundingRect() const
  {
    // TODO : cache and update when handles are moved
    QPointF topLeft = QPointF( 1, 1 ) * std::numeric_limits<qreal>::max();
    QPointF botRight = QPointF( 1, 1 ) * ( -std::numeric_limits<qreal>::max() );
    if( m_parent->m_curve != NULL )
    {
      size_t hc = m_parent->m_curve->getHandleCount();
      if( hc <= 1 )
      {
        QPointF pos;
        if( hc == 1 )
          pos = m_parent->m_curve->getHandle( 0 ).pos;
        m_boundingRect = QRectF( -1 + pos.x(), -1 + pos.y(), 2, 2 );
        return;
      }
      for( size_t i = 0; i < hc; i++ )
      {
        Handle h = m_parent->m_curve->getHandle( i );
        topLeft = min( topLeft, h.pos );
        botRight = max( botRight, h.pos );
        topLeft.setY( std::min( topLeft.y(), h.pos.y() - h.tanIn.y() ) );
        topLeft.setY( std::min( topLeft.y(), h.pos.y() + h.tanOut.y() ) );
        botRight.setY( std::max( botRight.y(), h.pos.y() - h.tanIn.y() ) );
        botRight.setY( std::max( botRight.y(), h.pos.y() + h.tanOut.y() ) );
      }
    }
    m_boundingRect = QRectF( topLeft, botRight );
    m_boundingRectDirty = false;
  }

  inline void updateBoundingRectIfDirty() const
  {
    if( m_boundingRectDirty )
      this->updateBoundingRect();
  }

public:
  FCurveShape( const FCurveItem* parent )
    : m_parent( parent )
    , m_boundingRectDirty( true )
  {
    this->updateBoundingRect();
  }

  QRectF boundingRect() const FTL_OVERRIDE
  {
    this->updateBoundingRectIfDirty();
    QRectF r = m_boundingRect;
    const qreal w = r.width();
    r.setLeft( r.left() - 100 * w );
    r.setRight( r.right() + 100 * w );
    return r;
  }

  inline QRectF keysBoundingRect() const
  {
    this->updateBoundingRectIfDirty();
    return m_boundingRect;
  }

  inline void setBoundingRectDirty()
  {
    this->prepareGeometryChange();
    m_boundingRectDirty = true;
    this->update();
  }

  void paint(
    QPainter * painter,
    const QStyleOptionGraphicsItem * option,
    QWidget * widget
  ) FTL_OVERRIDE
  {
    if( m_parent->m_curve != NULL )
    {
      const QRectF er = painter->transform().inverted()
        .mapRect( QRectF( widget->geometry() ) ).intersected( this->boundingRect() );
      QPen pen; pen.setCosmetic( true );
      pen.setWidthF( 2 );

      QColor inactiveColor( 80, 98, 110 );
      QColor activeColor( 102, 125, 140 );

      if( !this->m_parent->m_selectedHandles.empty() ) {
        pen.setColor( activeColor );        
      }
      else {
        pen.setColor( inactiveColor );        
      }

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
      //painter->drawRect( this->boundingRect() );
    }
  }
};

QRectF FCurveItem::keysBoundingRect() const { return m_curveShape->keysBoundingRect(); }

class FCurveItem::HandleWidget : public QGraphicsWidget
{
  FCurveItem* m_parent;
  size_t m_index;

  class Center : public QGraphicsRectItem
  {
    HandleWidget* m_parent;
    typedef QGraphicsRectItem Parent;
    bool m_selected;
    bool m_hovered;
    bool m_hasMovedBeforeRelease;
    void updateColor()
    {
      this->setPen( Qt::NoPen );
      this->setBrush( m_hovered ? QColor( 191, 191, 191 ) :
        m_selected ? QColor( 39, 168, 223 )
        : QColor( 128, 128, 128 )
      );
    }
  public:
    Center( HandleWidget* parent )
      : QGraphicsRectItem( parent )
      , m_parent( parent )
      , m_selected( false )
      , m_hovered( false )
    {
      this->setRect( QRectF( -4, -4, 8, 8 ) );
      this->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
      this->setAcceptHoverEvents( true );
      this->updateColor();
    };
    void setHandleSelected( bool selected )
    {
      m_selected = selected;
      this->updateColor();
    }
  protected:
    void mousePressEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
    {
      m_hasMovedBeforeRelease = false;
      if( !m_selected )
        m_parent->m_parent->clearHandleSelection();
      m_parent->m_parent->addHandleToSelection( m_parent->m_index );
      emit m_parent->m_parent->interactionBegin();
      this->setCursor( Qt::SizeAllCursor );
    }
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
    {
      AbstractFCurveModel* curve = m_parent->m_parent->m_curve;
      const size_t index = m_parent->m_index;
      const Handle h = curve->getHandle( index );
      m_hasMovedBeforeRelease = true;
      m_parent->m_parent->moveSelectedHandles( event->scenePos() - h.pos );
    }
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
    {
      Parent::mouseReleaseEvent( event );
      if( !m_hasMovedBeforeRelease )
      {
        m_parent->m_parent->clearHandleSelection();
        m_parent->m_parent->addHandleToSelection( m_parent->m_index );
        m_parent->m_parent->editHandle( m_parent->m_index );
      }
      emit m_parent->m_parent->interactionEnd();
      this->unsetCursor();
    }
    void hoverEnterEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE
    {
      m_hovered = true;
      this->updateColor();
    }
    void hoverLeaveEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE
    {
      m_hovered = false;
      this->updateColor();
    }
  };
  Center* m_center;

  class Tangent
  {
    bool m_inNotOut;
    QGraphicsLineItem* m_line;

    class End : public QGraphicsEllipseItem
    {
      HandleWidget* m_parent;
      Tangent* m_tangent;
      bool m_selected;
      typedef QGraphicsEllipseItem Parent;
      inline void updateColor()
      {
        this->setPen( Qt::NoPen );
        this->setBrush( m_selected ? QColor( 39, 168, 223 ) : QColor( 255, 255, 255 ) );
      }
    public:
      QGraphicsWidget* m_posW; // Used for its position
      End( HandleWidget* parent, Tangent* tangent )
        : QGraphicsEllipseItem()
        , m_parent( parent )
        , m_tangent( tangent )
        , m_posW( new QGraphicsWidget( parent ) )
      {
        this->setParentItem( m_posW );
        this->setRect( QRectF( -4, -4, 8, 8 ) );
        this->setFlag( QGraphicsItem::ItemIgnoresTransformations, true );
        this->setSelected( false );
        this->setAcceptHoverEvents( true );
      }
      inline void setEndSelected( bool s ) { m_selected = s; this->updateColor(); }
    protected:
      void mousePressEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
      {
        emit m_parent->m_parent->interactionBegin();
        m_parent->m_parent->editHandle( m_parent->m_index, m_tangent->m_inNotOut ? TAN_IN : TAN_OUT );
      }
      void mouseMoveEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
      {
        AbstractFCurveModel* curve = m_parent->m_parent->m_curve;
        const size_t index = m_parent->m_index;
        Handle h = curve->getHandle( index );

        const qreal crossProd = std::abs( h.tanIn.x() * h.tanOut.y() - h.tanIn.y() * h.tanOut.x() );
        const qreal l2In = len2( h.tanIn );
        const qreal l2Out = len2( h.tanOut );
        const bool splitTangents = // moving tagents independently ?
          event->modifiers().testFlag( Qt::ShiftModifier ) ||
          ( crossProd * crossProd > 0.001 * l2In * l2Out ) ||
          l2In == 0 || l2Out == 0
        ;
        if( m_tangent->m_inNotOut )
        {
          h.tanIn = -( event->scenePos() - m_parent->scenePos() );
          if( h.tanIn.x() < 0 )
            h.tanIn.setX( 0 );
          if( !splitTangents )
            h.tanOut = h.tanIn * sqrt( l2Out / l2In );
        }
        else
        {
          h.tanOut = ( event->scenePos() - m_parent->scenePos() );
          if( h.tanOut.x() < 0 )
            h.tanOut.setX( 0 );
          if( !splitTangents )
            h.tanIn = h.tanOut * sqrt( l2In / l2Out );
        }
        curve->setHandle( index, h );
      }
      void hoverEnterEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE { this->setCursor( Qt::SizeAllCursor ); }
      void hoverLeaveEvent( QGraphicsSceneHoverEvent *event ) FTL_OVERRIDE { this->unsetCursor(); }
      void mouseReleaseEvent( QGraphicsSceneMouseEvent *event ) FTL_OVERRIDE
      {
        Parent::mouseReleaseEvent( event );
        emit m_parent->m_parent->interactionEnd();
      }
    };
    End* m_end;

  public:
    Tangent( HandleWidget* parent, bool inNotOut )
      : m_inNotOut( inNotOut )
      , m_line( new QGraphicsLineItem( parent ) )
      , m_end( new End( parent, this ) )
    {
      QPen linePen; linePen.setCosmetic( true );
      linePen.setColor( QColor( 255, 255, 255, 128 ) );
      linePen.setWidthF( 2 );
      m_line->setPen( linePen );
    }

    void setValue( const Handle& h )
    {
      const QPointF p = m_inNotOut ? -h.tanIn : h.tanOut;
      m_line->setLine( QLineF( QPointF( 0, 0 ), p ) );
      m_end->m_posW->setPos( p );
    }

    inline void setVisible( bool visible )
    {
      m_line->setVisible( visible );
      m_end->setVisible( visible );
    }

    inline void setSelected( bool s ) { m_end->setEndSelected( s ); }
  };
  Tangent m_inT, m_outT;

public:
  HandleWidget( FCurveItem* parent, size_t index )
    : QGraphicsWidget( parent )
    , m_parent( parent )
    , m_index( index )
    , m_center( new Center( this ) )
    , m_inT( this, true )
    , m_outT( this, false )
  {
    this->setEditState( NOTHING );
  }
  void setValue( const Handle& h )
  {
    this->setPos( h.pos );
    m_inT.setValue( h );
    m_outT.setValue( h );
  }
  inline void setIndex( size_t i ) { this->m_index = i; }

  inline void setEditState( HandleProp p )
  {
    const bool visible = ( p != NOTHING );
    m_inT.setVisible( visible );
    m_outT.setVisible( visible );
    m_center->setHandleSelected( p == CENTER );
    m_inT.setSelected( p == TAN_IN );
    m_outT.setSelected( p == TAN_OUT );
  }

  inline void setHandleSelected( bool selected )
  {
    m_center->setHandleSelected( selected );
    if( !selected )
      this->setEditState( NOTHING );
  }
};

FCurveItem::FCurveItem()
  : m_curve( NULL )
  , m_curveShape( new FCurveShape( this ) )
{
  m_curveShape->setParentItem( this );
}

void FCurveItem::clearHandleSelection()
{
  for( std::set<size_t>::const_iterator it = m_selectedHandles.begin(); it != m_selectedHandles.end(); it++ )
    m_handles[*it]->setHandleSelected( false );
  m_selectedHandles.clear();
  emit this->stopEditingHandle();
}

void FCurveItem::addHandleToSelection( size_t i )
{
  m_selectedHandles.insert( i );
  m_handles[i]->setHandleSelected( true );
  if( m_selectedHandles.size() == 1 )
    this->editHandle( *m_selectedHandles.begin() );
  else
  if( m_selectedHandles.size() == 2 )
  {
    m_handles[*m_selectedHandles.begin()]->setEditState( NOTHING );
    emit this->stopEditingHandle();
    m_handles[*m_selectedHandles.begin()]->setHandleSelected( true );
  }
}

void FCurveItem::rectangleSelect( const QRectF& r )
{
  this->clearHandleSelection();
  for( size_t i = 0; i < m_handles.size(); i++ )
    if( r.contains( m_handles[i]->scenePos() ) )
      this->addHandleToSelection( i );
}

void FCurveItem::editHandle( size_t i, HandleProp p )
{
  m_handles[i]->setEditState( p );
  m_editedHandle = i;
  m_editedHandleProp = p;
  emit this->startEditingHandle();
}

void FCurveItem::deleteSelectedHandles()
{
  if( m_selectedHandles.empty() )
    return;
  else
  if( m_selectedHandles.size() == 1 )
  {
    size_t index = *m_selectedHandles.begin();
    this->clearHandleSelection();
    m_curve->deleteHandle( index );
  }
  else
  {
    std::vector<size_t> orderedIndices;
    for( std::set<size_t>::const_iterator it = m_selectedHandles.begin(); it != m_selectedHandles.end(); it++ )
      orderedIndices.push_back( *it );
    this->clearHandleSelection();
    std::sort( orderedIndices.begin(), orderedIndices.end() );
    m_curve->deleteHandles( orderedIndices.data(), orderedIndices.size() );
  }
}

void FCurveItem::moveSelectedHandles( QPointF delta )
{
  if( m_selectedHandles.empty() )
    return;
  else
  if( m_selectedHandles.size() == 1 )
  {
    const size_t index = *m_selectedHandles.begin();
    Handle h = m_curve->getHandle( index );
    h.pos += delta;
    m_curve->setHandle( index, h );
    return;
  }
  else
  {
    // TODO (optimisation) : cache that vector<size_t> to avoid reallocation
    std::vector<size_t> indices;
    for( std::set<size_t>::const_iterator it = m_selectedHandles.begin(); it != m_selectedHandles.end(); it++ )
      indices.push_back( *it );
    m_curve->moveHandles( indices.data(), indices.size(), delta );
  }
}

void FCurveItem::addHandle( size_t i )
{
  HandleWidget* w = new HandleWidget( this, i );
  w->setValue( m_curve->getHandle( i ) );
  m_handles.push_back( w );
}

void FCurveItem::onHandleAdded()
{
  this->addHandle( m_handles.size() );
  m_curveShape->setBoundingRectDirty();
}

void FCurveItem::onHandleDeleted( size_t i )
{
  delete m_handles[i];
  for( size_t j = i; j < m_handles.size() - 1; j++ )
  {
    m_handles[j] = m_handles[j + 1];
    m_handles[j]->setIndex( j );
  }
  m_handles.resize( m_handles.size() - 1 );

  this->clearHandleSelection();

  m_curveShape->setBoundingRectDirty();
  if( i == m_editedHandle )
    emit this->stopEditingHandle();
}

void FCurveItem::onHandleMoved( size_t i )
{
  assert( i < m_handles.size() );
  assert( m_handles.size() == m_curve->getHandleCount() );
  m_handles[i]->setValue( m_curve->getHandle( i ) );
  m_curveShape->setBoundingRectDirty();
  if( i == m_editedHandle )
    emit this->editedHandleValueChanged();
}

void FCurveItem::setCurve( AbstractFCurveModel* curve )
{
  assert( curve != m_curve );
  m_curve = curve;
  QOBJECT_CONNECT( m_curve, SIGNAL, AbstractFCurveModel, handleMoved, ( size_t ), this, SLOT, FCurveItem, onHandleMoved, ( size_t ) );
  QOBJECT_CONNECT( m_curve, SIGNAL, AbstractFCurveModel, handleAdded, (), this, SLOT, FCurveItem, onHandleAdded, () );
  QOBJECT_CONNECT( m_curve, SIGNAL, AbstractFCurveModel, handleDeleted, ( size_t ), this, SLOT, FCurveItem, onHandleDeleted, ( size_t ) );

  // Clearing previous handles
  for( std::vector<HandleWidget*>::const_iterator it = m_handles.begin(); it < m_handles.end(); it++ )
    delete *it;
  m_handles.clear();

  emit this->stopEditingHandle();

  size_t hc = m_curve->getHandleCount();
  for( size_t i = 0; i < hc; i++ )
    this->addHandle( i );

  m_curveShape->setBoundingRectDirty();
}
