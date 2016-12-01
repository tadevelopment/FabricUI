//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "Config.h"

using namespace FabricUI::Util;
using namespace FTL;

#include <fstream>
#include <sstream>

Config::Config( const std::string fileName )
  : ConfigSection()
  , m_fileName( fileName )
{
  std::ifstream file( fileName );
  if ( file.is_open() )
  {
    std::stringstream buffer;
    buffer << file.rdbuf();
    m_content = buffer.str();
    if ( !m_content.empty() )
    {
      try
      {
        m_json = JSONObject::Decode( JSONStrWithLoc( m_content ) );
        return;
      }
      catch ( FTL::JSONException e)
      {
        printf(
          "Error in %s, malformed JSON : %s\n",
          fileName.data(),
          e.getDescCStr()
        );
      }
    }
  }
  {
    printf( "Config file \"%s\" does not exist : creating it\n", m_fileName.data() );
    m_json = new JSONObject();
  }
}

Config::~Config()
{
  std::ofstream file( m_fileName );
  file << m_json->encode();
  printf( "Exported \"%s\"\n", m_fileName.data() );
  delete m_json;
}

ConfigSection& ConfigSection::getOrCreateSection( const std::string name )
{
  if ( m_sections.find( name ) == m_sections.end() )
  {
    ConfigSection& newSection = m_sections[name];
    newSection.m_json = m_json->has( name ) ?
      m_json->get( name )->cast<JSONObject>() : new JSONObject();
    m_json->insert( name, newSection.m_json );
  }
  return m_sections[name];
}

// bool

template<>
JSONValue* ConfigSection::createValue( const bool v ) const
{
  return new JSONBoolean( v );
}

template<>
bool ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getBooleanValue();
}

// int

template<>
JSONValue* ConfigSection::createValue( const int v ) const
{
  return new JSONSInt32( v );
}

template<>
int ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getSInt32Value();
}

// unsigned int

template<>
JSONValue* ConfigSection::createValue( const unsigned int v ) const
{
  return new JSONSInt32( v );
}

template<>
unsigned int ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getSInt32Value();
}

// double

template<>
JSONValue* ConfigSection::createValue( const double v ) const
{
  return new JSONFloat64( v );
}

template<>
double ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getFloat64Value();
}

// float

template<>
JSONValue* ConfigSection::createValue( const float v ) const
{
  return new JSONFloat64( v );
}

template<>
float ConfigSection::getValue( const JSONValue* entry ) const
{
  return entry->getFloat64Value();
}

// QString

template<>
JSONValue* ConfigSection::createValue( const QString v ) const
{
  return new JSONString( StrRef( v.toUtf8().data(), v.toUtf8().size() ) );
}

template<>
QString ConfigSection::getValue( const JSONValue* entry ) const
{
  StrRef v = entry->getStringValue();
  return QString::fromUtf8( v.data(), v.size() );
}

#include <QColor>

template<>
JSONValue* ConfigSection::createValue( const QColor v ) const
{
  JSONObject* color = new JSONObject();
  color->insert( "r", new JSONSInt32( v.red() ) );
  color->insert( "g", new JSONSInt32( v.green() ) );
  color->insert( "b", new JSONSInt32( v.blue() ) );
  return color;
}

template<>
QColor ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  return QColor(
    obj->getSInt32( "r" ),
    obj->getSInt32( "g" ),
    obj->getSInt32( "b" )
  );
}

#include <QFont>

template<>
JSONValue* ConfigSection::createValue( const QFont v ) const
{
  JSONObject* font = new JSONObject();

  // mandatory parameters (see the constructor in getValue, below)
  font->insert( "family", createValue( v.family() ) );
  font->insert( "pointSize", createValue( v.pointSize() ) );
  font->insert( "weight", createValue( v.weight() ) );

  if ( v.pixelSize() > 0 ) font->insert( "pixelSize", createValue( v.pixelSize() ) );
  if( v.pointSizeF() > 0 ) font->insert( "pointSizeF", createValue( v.pointSizeF() ) );
  font->insert( "styleHint", createValue<int>( v.styleHint() ) );
  font->insert( "hintingPreference", createValue<int>( v.hintingPreference() ) );
  return font;
}

template<>
QFont ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  QFont result = QFont(
    getValue<QString>( obj->get( "family" ) ),
    obj->getFloat64( "pointSize" ),
    obj->getSInt32( "weight" )
  );

  if ( obj->has( "pixelSize" ) )
    result.setPixelSize( obj->getSInt32( "pixelSize" ) );

  if ( obj->has( "styleHint" ) )
    result.setStyleHint( QFont::StyleHint( obj->getSInt32( "styleHint" ) ) );

  if( obj->has( "hintingPrefrence" ) )
    result.setHintingPreference( QFont::HintingPreference( obj->getSInt32(  "hintingPrefrence" ) ) );

  return result;
}
