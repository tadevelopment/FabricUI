#include <iostream>

#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QTextStream>
#include "DFGPresetSearchWidget.h"

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

// Copy of <FabricUI/Style/FabricStyle>
class Style : public QProxyStyle
{
public:

  Style() : QProxyStyle( QStyleFactory::create( "windows" ) )
  {
  }

  void polish( QPalette &pal ) FTL_OVERRIDE
  {
    QColor baseColor = QColor( 60, 60, 60 );
    QColor highlightColor = QColor( 240, 240, 240 );
    float brightnessSpread = 4.5f;
    float spread = brightnessSpread;

    if( baseColor.toHsv().valueF() > 0.5 )
      spread = 100.0f / brightnessSpread;
    else
      spread = 100.0f * brightnessSpread;

    QColor highlightedTextColor;
    if( highlightColor.toHsv().valueF() > 0.6 )
      highlightedTextColor = baseColor.darker( int( spread * 2 ) );
    else
      highlightedTextColor = baseColor.lighter( int( spread * 2 ) );

#define PALETTE_SET_BRUSH( parameter, defaultValue ) \
    pal.setBrush( parameter, defaultValue )

    PALETTE_SET_BRUSH( QPalette::Background, QBrush( baseColor ) );
    PALETTE_SET_BRUSH( QPalette::Window, QBrush( baseColor ) );
    PALETTE_SET_BRUSH( QPalette::Foreground, baseColor.lighter( int( spread ) ) );
    PALETTE_SET_BRUSH( QPalette::WindowText, baseColor.lighter( int( spread ) ) );
    PALETTE_SET_BRUSH( QPalette::Base, baseColor );
    PALETTE_SET_BRUSH( QPalette::AlternateBase, baseColor.darker( int( spread ) ) );
    PALETTE_SET_BRUSH( QPalette::ToolTipBase, baseColor );
    PALETTE_SET_BRUSH( QPalette::ToolTipText, baseColor.lighter( int( spread ) ) );
    PALETTE_SET_BRUSH( QPalette::Text, baseColor.lighter( int( spread*1.2 ) ) );
    PALETTE_SET_BRUSH( QPalette::Button, baseColor );
    PALETTE_SET_BRUSH( QPalette::ButtonText, baseColor.lighter( int( spread ) ) );
    PALETTE_SET_BRUSH( QPalette::BrightText, QColor( 240, 240, 240 ) );
    PALETTE_SET_BRUSH( QPalette::Light, baseColor.lighter( int( spread / 2 ) ) );
    PALETTE_SET_BRUSH( QPalette::Midlight, baseColor.lighter( int( spread / 4 ) ) );
    PALETTE_SET_BRUSH( QPalette::Dark, baseColor.darker( int( spread / 4 ) ) );
    PALETTE_SET_BRUSH( QPalette::Mid, baseColor );
    PALETTE_SET_BRUSH( QPalette::Shadow, baseColor.darker( int( spread / 2 ) ) );
    PALETTE_SET_BRUSH( QPalette::Highlight, highlightColor );
    PALETTE_SET_BRUSH( QPalette::HighlightedText, highlightedTextColor );
  }
};

void main( int argc, char** argv )
{
  TabSearch::ResultsView::UnitTest( "./" );

  QApplication* app = new QApplication( argc, argv );
  app->setStyle( new Style() );

  // Core Client
  FabricCore::Client::CreateOptions createOptions = {};
  createOptions.guarded = true;
  FabricCore::Client client( &ReportCallBack, 0, &createOptions );
  FabricCore::DFGHost host = client.getDFGHost();

  // TabSearch
  DFGPresetSearchWidget* w = new DFGPresetSearchWidget( &host );
  QFile styleSheet( "TabSearch.qss" );
  styleSheet.open( QIODevice::ReadOnly );
  w->setStyleSheet( QTextStream( &styleSheet ).readAll() );
  w->showForSearch( QPoint( 500, 500 ) );

  app->exec();
}
