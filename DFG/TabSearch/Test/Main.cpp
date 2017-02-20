#include <iostream>

#include <QApplication>
#include "DFGTabSearchWidget.h"

void ReportCallBack(
  void *userdata,
  FEC_ReportSource source,
  FEC_ReportLevel level,
  char const *data,
  uint32_t size
)
{
  std::cout << std::string( data, size ).c_str() << std::endl;
}

using namespace FabricUI::DFG;

void main( int argc, char** argv )
{
  QApplication* app = new QApplication( argc, argv );

  // Core Client
  FabricCore::Client::CreateOptions createOptions = {};
  createOptions.guarded = true;
  FabricCore::Client client( &ReportCallBack, 0, &createOptions );
  FabricCore::DFGHost host = client.getDFGHost();

  // TabSearch
  DFGTabSearchWidget2* w = new DFGTabSearchWidget2( &host );
  w->showForSearch( QPoint( 500, 500 ) );

  app->exec();
}
