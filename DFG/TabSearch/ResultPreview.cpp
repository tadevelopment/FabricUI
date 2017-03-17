// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultPreview.h"

#include "ItemView.h"

#include <FabricCore.h>
#include <QueryEdit.h>
#include <iostream>
#include <fstream>
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
  QLabel* m_header;

public:
  Section( const std::string& name )
    : m_header( new QLabel() )
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

  lay->addWidget( new Section( "Tags" ) );
  lay->addWidget( new Section( "Ports" ) );
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
  
  this->adjustSize();
}

const std::string& ResultPreview::getPreset() const { return m_preset; }
