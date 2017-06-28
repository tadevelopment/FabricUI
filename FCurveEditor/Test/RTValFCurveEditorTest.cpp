
#include <FabricUI/FCurveEditor/RuledGraphicsView.h>
#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FabricUI/FCurveEditor/Models/AnimXKL/RTValAnimXFCurveModel.h>

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
  RTValAnimXFCurveModel curve;

  {
    QGraphicsScene* scene = new QGraphicsScene();
    scene->setSceneRect( QRectF( -1E3, -1E3, 2 * 1E3, 2 * 1E3 ) );
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
