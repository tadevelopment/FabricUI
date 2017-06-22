
#include <FabricUI/FCurveEditor/RuledGraphicsView.h>

#include <QApplication>
#include <QGraphicsItem>

using namespace FabricUI::FCurveEditor;

inline float RandFloat() { return float( rand() ) / RAND_MAX; }

void main()
{
  int argc = 0;
  QApplication app( argc, NULL );
  {
    QGraphicsScene* scene = new QGraphicsScene();
    //scene->setSceneRect( QRectF( -1E3, -1E3, 1E3, 1E3 ) );
    for( size_t i = 0; i < 100; i++ )
    {
      QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem();
      float scale = 1E-2 + 0.1 *RandFloat();
      ellipse->setRect(
        QRectF(
          RandFloat(),
          RandFloat(),
          scale,
          scale
        )
      );
      ellipse->setBrush( QColor( 255, 128, 0 ) );
      QPen pen; pen.setWidthF( 0.1 * scale );
      ellipse->setPen( pen );
      scene->addItem( ellipse );
    }

    RuledGraphicsView* view = new RuledGraphicsView();
    view->setScene( scene );
    view->resize( 800, 600 );
    view->fitInView( 0, 0, 1, 1, Qt::KeepAspectRatio );
    view->show();
  }
  app.exec();
}
