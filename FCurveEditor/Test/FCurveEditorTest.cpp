
#include <FabricUI/FCurveEditor/FCurveEditor.h>
#include <FabricUI/FCurveEditor/Models/AnimXCpp/AnimXFCurveModel.h>

#include <QApplication>
#include <QDebug>

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
    const qreal xScale = 85.3;
    const qreal yScale = 0.01;
    h.pos = QPointF( 132 + xScale * float( i ) / nh, yScale * RandFloat() - 17.3 );
    h.tanIn = QPointF( xScale * RandFloat(), yScale * ( 1 - 2 * RandFloat() ) ) * 0.3;
    h.tanOut = ( rand() % 2 == 0 ? h.tanIn :
      QPointF( xScale * ( 1 - 2 * RandFloat() ), yScale * ( 1 - 2 * RandFloat() ) ) * 0.3 );
    curve.addHandle( h );
  }

  FCurveEditor* editor = new FCurveEditor();
  editor->setModel( &curve );
  editor->resize( 800, 600 );
  editor->show();

  app.exec();
  return 0;
}
