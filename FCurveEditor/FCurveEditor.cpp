//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "FCurveEditor.h"
#include "FCurveItem.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDebug>

using namespace FabricUI::FCurveEditor;

FCurveEditor::FCurveEditor()
  : m_model( NULL )
  , m_scene( new QGraphicsScene() )
  , m_curveItem( new FCurveItem() )
{
  m_scene->setSceneRect( QRectF( -1E8, -1E8, 2 * 1E8, 2 * 1E8 ) );
  this->view()->setScene( m_scene );
  m_scene->addItem( m_curveItem );
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
  }
  else
    Parent::mousePressEvent( e );
}
