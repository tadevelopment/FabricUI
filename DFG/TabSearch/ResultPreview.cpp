// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultPreview.h"

#include "ItemView.h"

#include <FabricCore.h>
#include "QueryEdit.h"
#include <iostream>
#include <fstream>
#include <QTableWidget>
#include <QHeaderView>
#include <QLayout>
#include <QLabel>
#include <FTL/JSONValue.h>

using namespace FabricUI::DFG::TabSearch;
using namespace FabricCore;

struct Port
{
  std::string type, name;
};

struct PresetDetails
{
  std::string description;
  std::vector<Port> ports;
  std::vector<Query::Tag> tags;
};

PresetDetails GetDetails(
  const std::string& preset,
  DFGHost* host
)
{
  PresetDetails details;
  std::string filePath = host->getPresetImportPathname( preset.data() );
  
  try
  {
    // Reading the .canvas as a string
    std::ifstream file( filePath.data() );
    if( !file.is_open() )
      std::cerr << "Error : couldn't open preset \"" << filePath << "\"" << std::endl;
    else
    {
      std::string content = std::string(
        std::istreambuf_iterator<char>( file ),
        std::istreambuf_iterator<char>()
      );

      // Parsing the .canvas
      const FTL::JSONValue* json = FTL::JSONValue::Decode( content.data() );
      const FTL::JSONObject* presetJs = json->cast<FTL::JSONObject>();
      if( presetJs->has( "metadata" ) )
      {
        const FTL::JSONObject* metadata = presetJs->getObject( "metadata" );
        if( metadata->has( "uiTooltip" ) )
          details.description = metadata->getString( "uiTooltip" );
      }
      if( presetJs->has( "ports" ) )
      {
        const FTL::JSONArray* ports = presetJs->getArray( "ports" );
        for( FTL::JSONArray::const_iterator it = ports->begin(); it != ports->end(); it++ )
        {
          const FTL::JSONObject* portJs = ( *it )->cast<FTL::JSONObject>();
          Port port;
          if( portJs->has( "typeSpec" ) )
            port.type = portJs->getString( "typeSpec" );
          port.name = portJs->getString( "name" );
          details.ports.push_back( port );
        }
      }
      delete json;
    }

    // Fetching tags from the DFGHost
    {
      FEC_StringRef tagsStrR = FEC_DFGHostGetPresetTags( host->getFECDFGHostRef(), preset.data() );
      FTL::StrRef tagsStr( FEC_StringGetCStr( tagsStrR ), FEC_StringGetSize( tagsStrR ) );
      FTL::JSONValue* tags = FTL::JSONValue::Decode( tagsStr );
      FTL::JSONArray* tagsA = tags->cast<FTL::JSONArray>();

      for( FTL::JSONArray::const_iterator it = tagsA->begin(); it != tagsA->end(); it++ )
        details.tags.push_back( std::string( ( *it )->getStringValue() ) );

      delete tags;
    }
  }
  catch( const FTL::JSONException& e )
  {
    std::cerr << preset << "; Error : " << e.getDescCStr() << std::endl;
    assert( false );
  }

  return details;
}

inline QString Bold( const QString& s ) { return "<b>" + s + "</b>"; }

class Section : public QWidget
{
  void toggleCollapse();

  struct Header : public QLabel
  {
    typedef QLabel Parent;

    Section *m_parent;
    Header( Section* parent ) : m_parent( parent ) {}

    void mouseReleaseEvent( QMouseEvent *ev ) FTL_OVERRIDE
    {
      Parent::mouseReleaseEvent( ev );
      m_parent->toggleCollapse();
    }
  };

  Header* m_header;
  QWidget* m_widget;

public:
  Section( const std::string& name )
    : m_header( new Header( this ) )
    , m_widget( NULL )
  {
    this->setObjectName( "Section" );
    m_header->setObjectName( "Header" );

    this->setLayout( new QVBoxLayout() );
    this->layout()->setMargin( 0 );
    this->layout()->setSpacing( 0 );
    this->layout()->addWidget( m_header );

    m_header->setText( Bold( QString::fromStdString( name ) ) );
    m_header->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
  }

  // takes ownership of the widget
  void setWidget( QWidget* widget )
  {
    if( m_widget != NULL )
    {
      this->layout()->removeWidget( m_widget );
      m_widget->deleteLater();
    }
    m_widget = widget;
    this->layout()->addWidget( m_widget );
  }
};

void Section::toggleCollapse()
{
  if( m_widget != NULL )
    m_widget->setVisible( !m_widget->isVisible() );
}

class ResultPreview::PortsView : public QTableWidget
{
public:

  PortsView()
  {
    this->setFocusPolicy( Qt::NoFocus );
    this->setColumnCount( 2 );
    QStringList headerLabels;
    headerLabels.push_back( "Name" ); headerLabels.push_back( "Type" );
    this->setHorizontalHeaderLabels( headerLabels );
    this->verticalHeader()->hide();
    this->setEditTriggers( QAbstractItemView::NoEditTriggers );
  }

  void setPorts( const std::vector<Port>& ports )
  {
    this->setRowCount( ports.size() );
    for( size_t i = 0; i< ports.size(); i++ )
    {
      const Port& p = ports[i];
      for( size_t j = 0; j < 2; j++ )
      {
        QTableWidgetItem* item =
          new QTableWidgetItem( QString::number( i ) + ";" + QString::number( j ) );
        this->setItem( i, j, item );
        item->setData( Qt::DisplayRole, QString::fromStdString( j == 0 ? p.name : p.type ) );
      }
    }
  }
};

ResultPreview::ResultPreview( FabricCore::DFGHost* host )
  : m_host( host )
  , m_name( new QLabel() )
  , m_description( new QLabel() )
{
  this->setObjectName( "ResultPreview" );
  m_name->setObjectName( "Name" );
  m_description->setObjectName( "Description" );

  clear();
  QVBoxLayout* lay = new QVBoxLayout();
  lay->setMargin( 0 );
  lay->setAlignment( Qt::AlignTop );
  this->setLayout( lay );

  lay->addWidget( m_name );
  lay->addWidget( m_description );
  m_description->setWordWrap( true );

  Section* ports = new Section( "Ports" );
  m_portsTable = new PortsView();
  ports->setWidget( m_portsTable );
  lay->addWidget( ports );

  lay->addWidget( new Section( "Tags" ) );
}

void ResultPreview::clear()
{
  m_preset = "";
  m_name->setText( "" );
}

void ResultPreview::setPreset( const std::string& preset )
{
  if( m_preset == preset )
    return;

  m_preset = preset;
  std::string name = m_preset.substr( m_preset.rfind( '.' ) + 1 );
  m_name->setText( Bold( QString::fromStdString( name ) ) );

  PresetDetails details = GetDetails( getPreset(), m_host );
  m_description->setText( QString::fromStdString( details.description ) );

  m_portsTable->setPorts( details.ports );
  
  this->adjustSize();
}

const std::string& ResultPreview::getPreset() const { return m_preset; }
