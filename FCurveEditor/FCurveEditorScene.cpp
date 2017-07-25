
#include <FabricUI/FCurveEditor/FCurveEditorScene.h>
#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <FabricUI/FCurveEditor/FCurveItem.h>

#include <FabricUI/Util/QtSignalsSlots.h>

#include <QWidget>
#include <QGraphicsSceneEvent>

using namespace FabricUI::FCurveEditor;

FCurveEditorScene::FCurveEditorScene( AbstractFCurveModel* model )
  : m_curveItem( new FCurveItem( this ) )
  , m_isDraggingKey( false )
{
  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, interactionBegin, ( ), this, SIGNAL, FCurveEditorScene, interactionBegin, ( ) );
  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, interactionEnd, ( ), this, SIGNAL, FCurveEditorScene, interactionEnd, ( ) );

  m_curveItem->setCurve( model );
  this->addItem( m_curveItem );

  this->setSceneRect( QRectF( -1E8, -1E8, 2 * 1E8, 2 * 1E8 ) );
  this->setMode( SELECT );
}

void FCurveEditorScene::setMode( Mode m )
{
  m_mode = m;
  if( m_mode == REMOVE )
    m_curveItem->clearKeySelection();
  emit this->modeChanged();
}

void FCurveEditorScene::mousePressEvent( QGraphicsSceneMouseEvent * e )
{
  if( this->mode() == ADD && e->button() == Qt::LeftButton )
  {
    // Adding a new Key
    QPointF scenePos = e->scenePos();
    Key h; h.pos = scenePos;
    h.tanIn = QPointF( 1, 0 ); h.tanOut = h.tanIn;
    h.tanInType = 0; h.tanOutType = 0;
    m_curveItem->curve()->addKey( h );
    size_t index = m_curveItem->curve()->getKeyCount() - 1;
    m_curveItem->curve()->autoTangents( index );
    m_curveItem->clearKeySelection();
    m_curveItem->addKeyToSelection( index );
    m_isDraggingKey = true;
    emit this->interactionBegin();
  }
  else
    Parent::mousePressEvent( e );
}

void FCurveEditorScene::mouseMoveEvent( QGraphicsSceneMouseEvent * e )
{
  Parent::mouseMoveEvent( e );
  if( m_isDraggingKey )
  {
    size_t index = m_curveItem->curve()->getKeyCount() - 1;
    Key k = m_curveItem->curve()->getKey( index );
    k.pos = e->scenePos();
    m_curveItem->curve()->setKey( index, k );
    e->widget()->repaint();
  }
}

void FCurveEditorScene::mouseReleaseEvent( QGraphicsSceneMouseEvent * e )
{
  Parent::mouseReleaseEvent( e );
  m_isDraggingKey = false;
  emit this->interactionEnd();
}
