//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "Config.h"

using namespace FabricUI::Util;

#include <QFile>
#include <QTextStream>

Config::Config( const QString fileName )
  : ConfigSection()
  , m_fileName( fileName )
{
  QFile file( m_fileName );
  if ( file.exists() )
  {
    file.open( QIODevice::ReadOnly );
    assert( file.isReadable() );

    QTextStream text( &file );
    m_content = text.readAll();
    if ( !m_content.isEmpty() )
    {
      try
      {
        m_json = JSONObject::Decode( JSONStrWithLoc( m_content.toStdString().data() ) );
        return;
      }
      catch ( FTL::JSONException e)
      {
        printf(
          "Error in %s, malformed JSON : %s\n",
          fileName.toStdString().data(),
          e.getDescCStr()
        );
      }
    }
  }
  {
    printf( "Config file \"%s\" does not exist : creating it\n", m_fileName.toStdString().data() );
    m_json = new JSONObject();
  }
}

Config::~Config()
{
  QFile file( m_fileName );
  file.open( QIODevice::WriteOnly );
  assert( file.isWritable() );
  printf( "Exported \"%s\"\n", m_fileName.toStdString().data() );
  QTextStream text( &file );
  text << m_json->encode().data();
  delete m_json;
}

template <typename T>
T ConfigSection::_getOrCreateValue( const QString key, const T defaultValue )
{
  if ( !m_json->has( key.toStdString() ) )
  {
    m_json->insert( key.toStdString(), createValue<T>( defaultValue ) );
    return defaultValue;
  }
  try
  {
    return getValue<T>( m_json->get( key.toStdString() ) );
  }
  catch ( FTL::JSONException e )
  {
    printf(
      "Error : malformed entry for key \"%s\" : \"%s\"\n",
      key.toStdString(),
      m_json->get( key.toStdString() )->encode().data()
    );
    return defaultValue;
  }
}

ConfigSection& ConfigSection::getOrCreateSection( const QString name )
{
  if ( m_sections.find( name ) == m_sections.end() )
  {
    ConfigSection& newSection = m_sections[name];
    newSection.m_json = m_json->has( name.toStdString() ) ?
      m_json->get( name.toStdString() )->cast<JSONObject>() : new JSONObject();
    m_json->insert( name.toStdString(), newSection.m_json );
  }
  return m_sections[name];
}

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

template <>
double ConfigSection::getOrCreateValue( const QString key, const double defaultValue )
{ return ConfigSection::_getOrCreateValue( key, defaultValue ); }

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

template <>
QColor ConfigSection::getOrCreateValue( const QString key, const QColor defaultValue )
{ return ConfigSection::_getOrCreateValue( key, defaultValue ); }

#include <QFont>

template<>
JSONValue* ConfigSection::createValue( const QFont v ) const
{
  JSONObject* font = new JSONObject();
  font->insert( "family", new JSONString( v.family().toStdString() ) );
  if ( v.pixelSize() > 0 ) font->insert( "pixelSize", new JSONSInt32( v.pixelSize() ) );
  font->insert( "weight", new JSONSInt32( v.weight() ) );
  font->insert( "styleHint", new JSONSInt32( v.styleHint() ) );
  return font;
}

template<>
QFont ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  QFont result;
  if ( obj->has( "family" ) ) result.setFamily( QString::fromStdString( obj->getString( "family" ) ) );
  if ( obj->has( "pixelSize" ) ) result.setPixelSize( obj->getSInt32( "pixelSize" ) );
  if ( obj->has( "weight" ) ) result.setWeight( obj->getSInt32( "weight" ) );
  if ( obj->has( "styleHint" ) ) result.setStyleHint( QFont::StyleHint( obj->getSInt32( "styleHint" ) ) );
  return result;
}

template <>
QFont ConfigSection::getOrCreateValue( const QString key, const QFont defaultValue )
{ return ConfigSection::_getOrCreateValue( key, defaultValue ); }
