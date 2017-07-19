//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "FCurveEditor.h"
#include "FCurveItem.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>

#include <QDebug>
#include <assert.h>

#include <FabricUI/Util/QtSignalsSlots.h>

using namespace FabricUI::FCurveEditor;

class FCurveEditor::KeyValueEditor : public QFrame
{
  FCurveEditor* m_parent;

  class FloatEditor : public QWidget
  {
    KeyValueEditor* m_parent;
    QLabel* m_label;
    QLineEdit* m_edit;
  public:
    FloatEditor( KeyValueEditor* parent, bool isXNotY )
      : QWidget( parent )
      , m_parent( parent )
      , m_label( new QLabel( isXNotY ? "X:" : "Y:" ) )
      , m_edit( new QLineEdit() )
    {
      this->setObjectName( "FloatEditor" );

      QHBoxLayout* m_layout = new QHBoxLayout();
      m_layout->setMargin( 0 );
      m_layout->addWidget( m_label );
      m_layout->addWidget( m_edit );
      this->setLayout( m_layout );
      this->set( 0 );

      if( isXNotY )
        QOBJECT_CONNECT( m_edit, SIGNAL, QLineEdit, editingFinished, ( ), m_parent->m_parent, SLOT, FCurveEditor, veXEditFinished, ( ) );
      else
        QOBJECT_CONNECT( m_edit, SIGNAL, QLineEdit, editingFinished, ( ), m_parent->m_parent, SLOT, FCurveEditor, veYEditFinished, ( ) );
    }
    inline void set( qreal v )
    {
      m_edit->setText( QString::number( v ) );
      m_edit->setCursorPosition( 0 );
    }
    inline QString get() const { return m_edit->text(); }
  };
public:
  FloatEditor* m_x;
  FloatEditor* m_y;

  KeyValueEditor( FCurveEditor* parent )
    : QFrame( parent )
    , m_parent( parent )
    , m_x( new FloatEditor( this, true ) )
    , m_y( new FloatEditor( this, false ) )
  {
    this->setObjectName( "KeyValueEditor" );

    QHBoxLayout* m_layout = new QHBoxLayout();
    m_layout->setContentsMargins( QMargins( 8, 2, 8, 2 ) );
    m_layout->setSpacing( 8 );
    m_layout->addWidget( m_x );
    m_layout->addWidget( m_y );
    this->setLayout( m_layout );
    this->resize( 200, 32 );
  }
};

void FCurveEditor::veEditFinished( bool isXNotY )
{
  const QString text = ( isXNotY ? m_keyValueEditor->m_x->get() : m_keyValueEditor->m_y->get() );
  bool ok;
  const qreal v = text.toDouble( &ok );
  if( !ok )
    this->onEditedKeyValueChanged();
  else
  {
    Key h = m_model->getKey( m_curveItem->editedKey() );
    QPointF* p = NULL;
    switch( m_curveItem->editedKeyProp() )
    {
    case FCurveItem::CENTER: p = &h.pos; break;
    case FCurveItem::TAN_IN: p = &h.tanIn; break;
    case FCurveItem::TAN_OUT: p = &h.tanOut; break;
    case FCurveItem::NOTHING: assert( false ); break;
    }
    if( isXNotY )
      p->setX( v );
    else
      p->setY( v );
    m_model->setKey( m_curveItem->editedKey(), h );
  }
}

FCurveEditor::FCurveEditor()
  : m_model( NULL )
  , m_scene( new QGraphicsScene() )
  , m_curveItem( new FCurveItem() )
  , m_keyValueEditor( new KeyValueEditor( this ) )
{
  this->setObjectName( "FCurveEditor" );

  m_scene->setSceneRect( QRectF( -1E8, -1E8, 2 * 1E8, 2 * 1E8 ) );
  this->view()->setScene( m_scene );
  m_scene->addItem( m_curveItem );

  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, interactionBegin, (), this, SIGNAL, FCurveEditor, interactionBegin, () );
  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, interactionEnd, (), this, SIGNAL, FCurveEditor, interactionEnd, () );
  QOBJECT_CONNECT(
    this, SIGNAL, FCurveEditor, rectangleSelectReleased, ( const QRectF&, Qt::KeyboardModifiers ),
    this, SLOT, FCurveEditor, onRectangleSelectReleased, ( const QRectF&, Qt::KeyboardModifiers )
  );
  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, startEditingKey, ( ), this, SLOT, FCurveEditor, onStartEditingKey, ( ) );
  QOBJECT_CONNECT(
    m_curveItem, SIGNAL, FCurveItem, editedKeyValueChanged, ( ),
    this, SLOT, FCurveEditor, onEditedKeyValueChanged, ( )
  );
  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, stopEditingKey, ( ), this, SLOT, FCurveEditor, onStopEditingKey, ( ) );
  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, repaintViews, ( ), this, SLOT, FCurveEditor, onRepaintViews, ( ) );

  QAction* frameAllAction = new QAction( "Frame All Keys", this );
  frameAllAction->setShortcutContext( Qt::WidgetWithChildrenShortcut );
  frameAllAction->setShortcut( Qt::Key_A );
  QOBJECT_CONNECT( frameAllAction, SIGNAL, QAction, triggered, (), this, SLOT, FCurveEditor, onFrameAllKeys, () );
  this->addAction( frameAllAction );

  QAction* frameSelectedAction = new QAction( "Frame Selected Keys", this );
  frameSelectedAction->setShortcutContext( Qt::WidgetWithChildrenShortcut );
  frameSelectedAction->setShortcut( Qt::Key_F );
  QOBJECT_CONNECT( frameSelectedAction, SIGNAL, QAction, triggered, (), this, SLOT, FCurveEditor, onFrameSelectedKeys, () );
  this->addAction( frameSelectedAction );

  QAction* deleteAction = new QAction( "Delete selected Keys", this );
  deleteAction->setShortcutContext( Qt::WidgetWithChildrenShortcut );
  deleteAction->setShortcut( Qt::Key_Delete );
  QOBJECT_CONNECT( deleteAction, SIGNAL, QAction, triggered, (), this, SLOT, FCurveEditor, onDeleteSelectedKeys, () );
  this->addAction( deleteAction );

  this->setVEPos( QPoint( -20, 20 ) );
}

void FCurveEditor::resizeEvent( QResizeEvent * e )
{
  Parent::resizeEvent( e );
  this->updateVEPos();
}

void FCurveEditor::updateVEPos()
{
  m_keyValueEditor->setGeometry( QRect(
    ( m_vePos.x() < 0 ?
      this->rect().right() + m_vePos.x() - m_keyValueEditor->width() :
      this->rect().left() + m_vePos.x()
    ),
    ( m_vePos.y() < 0 ?
      this->rect().bottom() + m_vePos.y() - m_keyValueEditor->height() :
      this->rect().top() + m_vePos.y()
    ),
    m_keyValueEditor->width(),
    m_keyValueEditor->height()
  ) );
}

void FCurveEditor::onEditedKeyValueChanged()
{
  Key h = m_model->getKey( m_curveItem->editedKey() );
  QPointF p;
  switch( m_curveItem->editedKeyProp() )
  {
  case FCurveItem::POSITION: p = h.pos; break;
  case FCurveItem::TAN_IN: p = h.tanIn; break;
  case FCurveItem::TAN_OUT: p = h.tanOut; break;
  case FCurveItem::NOTHING: assert( false ); break;
  }
  m_keyValueEditor->m_x->set( p.x() );
  m_keyValueEditor->m_y->set( p.y() );
}

void FCurveEditor::onStartEditingKey()
{
  m_keyValueEditor->setVisible( true );
  this->onEditedKeyValueChanged();
}

void FCurveEditor::onStopEditingKey()
{
  m_keyValueEditor->setVisible( false );
}

void FCurveEditor::onRectangleSelectReleased( const QRectF& r, Qt::KeyboardModifiers m )
{
  if( !m.testFlag( Qt::ShiftModifier ) && !m.testFlag( Qt::ControlModifier ) )
    m_curveItem->clearKeySelection();
  m_curveItem->rectangleSelect( r, m );
}

void FCurveEditor::onFrameAllKeys()
{
  this->frameAllKeys();
}

void FCurveEditor::onFrameSelectedKeys()
{
  this->frameSelectedKeys();
}

void FCurveEditor::onDeleteSelectedKeys()
{
  m_curveItem->deleteSelectedKeys();
}

FCurveEditor::~FCurveEditor()
{
  delete m_scene;
}

void FCurveEditor::setModel( AbstractFCurveModel* model )
{
  m_model = model;
  m_curveItem->setCurve( model );
  this->fitInView( m_curveItem->keysBoundingRect() );
}

void FCurveEditor::frameAllKeys()
{
  QRectF rect = m_curveItem->keysBoundingRect();
  if( rect.isValid() )
    this->Parent::fitInView( rect );
}

void FCurveEditor::frameSelectedKeys()
{
  QRectF rect = m_curveItem->selectedKeysBoundingRect();
  if( rect.isValid() )
    this->Parent::fitInView( rect );
}

void FCurveEditor::mousePressEvent( QMouseEvent * e )
{
  if( e->button() == Qt::RightButton )
  {
    // Adding a new Key
    QPointF scenePos = this->view()->mapToScene(
      this->view()->mapFromGlobal( this->mapToGlobal( e->pos() ) ) );
    m_model->addKey();
    Key h; h.pos = scenePos;
    {
      // heuristic for tangents, based on the current zoom level
      h.tanIn.setX( 20 / this->view()->transform().m11() );
      h.tanOut.setX( h.tanIn.x() );
    }
    m_model->setKey( m_model->getKeyCount() - 1, h );
    m_model->autoTangents( m_model->getKeyCount() - 1 );
  }
  else
    Parent::mousePressEvent( e );
}

void FCurveEditor::onRepaintViews()
{
  this->repaint();
}
