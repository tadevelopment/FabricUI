
#include <FabricUI/FCurveEditor/RuledGraphicsView.h>
#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FabricUI/FCurveEditor/AnimXFCurveModel.h>

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

using namespace FabricUI::FCurveEditor;

inline float RandFloat() { return float( rand() ) / RAND_MAX; }

int main()
{
  int argc = 0;
  QApplication app( argc, NULL );
  AnimxFCurveModel curve;

  size_t nh = 7;
  for( size_t i = 0; i < nh; i++ )
  {
    Handle h;
    h.pos = QPointF( float( i ) / nh, RandFloat() );
    h.tanIn = QPointF( RandFloat(), 1 - 2 * RandFloat() ) * 0.3;
    h.tanOut = ( rand() % 2 == 0 ? h.tanIn : QPointF( 1 - 2 * RandFloat(), 1 - 2 * RandFloat() ) * 0.3 );
    curve.addHandle( h );
  }

  {
    QGraphicsScene* scene = new QGraphicsScene();
    scene->setSceneRect( QRectF( -1E3, -1E3, 2*1E3, 2*1E3 ) );
    FCurveEditor* curveEditor = new FCurveEditor();
    curveEditor->setCurve( &curve );
    scene->addItem( curveEditor );

    RuledGraphicsView* view = new RuledGraphicsView();
    view->view()->setScene( scene );
    view->resize( 800, 600 );
    view->view()->fitInView( 0, 0, 1, 1, Qt::KeepAspectRatio );
    view->show();
  }
  app.exec();
  return 0;
}
