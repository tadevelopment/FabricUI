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

#include <FabricUI/Util/QtSignalsSlots.h>

using namespace FabricUI::FCurveEditor;

class FCurveEditor::ValueEditor : public QFrame
{
  FCurveEditor* m_parent;

  class FloatEditor : public QWidget
  {
    ValueEditor* m_parent;
    QLabel* m_label;
    QLineEdit* m_edit;
  public:
    FloatEditor( ValueEditor* parent, QString label )
      : QWidget( parent )
      , m_parent( parent )
      , m_label( new QLabel( label ) )
      , m_edit( new QLineEdit() )
    {
      QHBoxLayout* m_layout = new QHBoxLayout();
      m_layout->setMargin( 0 );
      m_layout->addWidget( m_label );
      m_layout->addWidget( m_edit );
      this->setLayout( m_layout );
      m_edit->setText( "0.0" );
      m_edit->setReadOnly( true ); // TODO : change

      // HACK : use QSS instead
      this->setStyleSheet( "background-color: none;" );
      m_edit->setStyleSheet( "background-color: rgba(0,0,0,200);" );
    }
  };
  FloatEditor* m_x;
  FloatEditor* m_y;

public:
  ValueEditor( FCurveEditor* parent )
    : QFrame( parent )
    , m_parent( parent )
    , m_x( new FloatEditor( this, "X:" ) )
    , m_y( new FloatEditor( this, "Y:" ) )
  {
    // HACK : use QSS instead
    this->setStyleSheet( "background-color: rgba(32,32,32,128); border-radius: 16px; color: #FFF;" );

    QHBoxLayout* m_layout = new QHBoxLayout();
    m_layout->addWidget( m_x );
    m_layout->addWidget( m_y );
    this->setLayout( m_layout );
    this->resize( 200, 80 );
  }
};

FCurveEditor::FCurveEditor()
  : m_model( NULL )
  , m_scene( new QGraphicsScene() )
  , m_curveItem( new FCurveItem() )
  , m_valueEditor( new ValueEditor( this ) )
{
  m_scene->setSceneRect( QRectF( -1E8, -1E8, 2 * 1E8, 2 * 1E8 ) );
  this->view()->setScene( m_scene );
  m_scene->addItem( m_curveItem );

  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, interactionBegin, (), this, SIGNAL, FCurveEditor, interactionBegin, () );
  QOBJECT_CONNECT( m_curveItem, SIGNAL, FCurveItem, interactionEnd, (), this, SIGNAL, FCurveEditor, interactionEnd, () );
  QOBJECT_CONNECT(
    this, SIGNAL, FCurveEditor, rectangleSelectReleased, ( const QRectF& ),
    this, SLOT, FCurveEditor, onRectangleSelectReleased, ( const QRectF& )
  );

  QAction* deleteAction = new QAction( "Delete selected Handles", this );
  deleteAction->setShortcutContext( Qt::WidgetWithChildrenShortcut );
  deleteAction->setShortcut( Qt::Key_Delete );
  QOBJECT_CONNECT( deleteAction, SIGNAL, QAction, triggered, (), this, SLOT, FCurveEditor, onDeleteSelectedHandles, () );
  this->addAction( deleteAction );
}

void FCurveEditor::resizeEvent( QResizeEvent * e )
{
  Parent::resizeEvent( e );
  m_valueEditor->setGeometry( QRect(
    this->rect().right() - 20 - m_valueEditor->width(),
    this->rect().top() + 20,
    m_valueEditor->width(),
    m_valueEditor->height()
  ) );
}

void FCurveEditor::onRectangleSelectReleased( const QRectF& r )
{
  m_curveItem->rectangleSelect( r );
}

void FCurveEditor::onDeleteSelectedHandles()
{
  m_curveItem->deleteSelectedHandles();
}

FCurveEditor::~FCurveEditor()
{
  delete m_scene;
}

void FCurveEditor::setModel( AbstractFCurveModel* model )
{
  m_model = model;
  m_curveItem->setCurve( model );
  this->fitInView( m_scene->itemsBoundingRect() );
}

void FCurveEditor::mousePressEvent( QMouseEvent * e )
{
  if( e->button() == Qt::RightButton )
  {
    // Adding a new Handle
    QPointF scenePos = this->view()->mapToScene(
      this->view()->mapFromGlobal( this->mapToGlobal( e->pos() ) ) );
    m_model->addHandle();
    Handle h; h.pos = scenePos;
    {
      // heuristic for tangents, based on the current zoom level
      h.tanIn.setX( 20 / this->view()->transform().m11() );
      h.tanOut.setX( h.tanIn.x() );
    }
    m_model->setHandle( m_model->getHandleCount() - 1, h );
    m_model->autoTangents( m_model->getHandleCount() - 1 );
  }
  else
    Parent::mousePressEvent( e );
}
