#include <iostream>

#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QTextStream>
#include <FabricUI/DFG/TabSearch/DFGPresetSearchWidget.h>
#include <FabricUI/Style/FabricStyle.h>

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
  TabSearch::ResultsView::UnitTest( "./" );

  QApplication* app = new QApplication( argc, argv );
  app->setStyle( new FabricUI::Style::FabricStyle() );

  // Core Client
  FabricCore::Client::CreateOptions createOptions = {};
  createOptions.guarded = true;
  FabricCore::Client client( &ReportCallBack, 0, &createOptions );
  FabricCore::DFGHost host = client.getDFGHost();

  // TabSearch
  DFGPresetSearchWidget* w = new DFGPresetSearchWidget( &host );
  w->registerVariable( "TestVar", "AType" );
  w->registerVariable( "Some.Path.And.A.Var", "Float32" );
  QFile styleSheet( "FabricUI.qss" );
  styleSheet.open( QIODevice::ReadOnly );
  w->setStyleSheet( QTextStream( &styleSheet ).readAll() );
  w->showForSearch( QPoint( 500, 500 ) );

  app->exec();
}
