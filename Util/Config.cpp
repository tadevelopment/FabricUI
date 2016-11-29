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

template <>
bool ConfigSection::getOrCreateValue( const QString key, const bool defaultValue )
{ return ConfigSection::_getOrCreateValue( key, defaultValue ); }

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

template <>
int ConfigSection::getOrCreateValue( const QString key, const int defaultValue )
{ return ConfigSection::_getOrCreateValue( key, defaultValue ); }

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

template <>
double ConfigSection::getOrCreateValue( const QString key, const double defaultValue )
{ return ConfigSection::_getOrCreateValue( key, defaultValue ); }

template <>
float ConfigSection::getOrCreateValue( const QString key, const float defaultValue )
{ return ConfigSection::_getOrCreateValue( key, double(defaultValue) ); }

// QString

template<>
JSONValue* ConfigSection::createValue( const QString v ) const
{
  return new JSONString( StrRef( v.toUtf8().data(), v.size() ) );
}

template<>
QString ConfigSection::getValue( const JSONValue* entry ) const
{
  StrRef v = entry->getStringValue();
  return QString::fromUtf8( v.data(), v.size() );
}

template <>
QString ConfigSection::getOrCreateValue( const QString key, const QString defaultValue )
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
  font->insert( "family", createValue( v.family() ) );
  if ( v.pixelSize() > 0 ) font->insert( "pixelSize", createValue( v.pixelSize() ) );
  font->insert( "pointSize", createValue( v.pointSizeF() ) );
  font->insert( "weight", createValue( v.weight() ) );
  font->insert( "styleHint", createValue<int>( v.styleHint() ) );
  font->insert( "hintingPreference", createValue<int>( v.hintingPreference() ) );
  return font;
}

template<>
QFont ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  QFont result;

  if ( obj->has( "family" ) )
    result.setFamily( QString::fromStdString( obj->getString( "family" ) ) );

  if ( obj->has( "pixelSize" ) )
    result.setPixelSize( obj->getSInt32( "pixelSize" ) );

  if ( obj->has( "pointSize" ) )
    result.setPointSizeF( obj->getFloat64( "pointSize" ) );

  if ( obj->has( "weight" ) )
    result.setWeight( obj->getSInt32( "weight" ) );

  if ( obj->has( "styleHint" ) )
    result.setStyleHint( QFont::StyleHint( obj->getSInt32( "styleHint" ) ) );

  if( obj->has( "hintingPrefrence" ) )
    result.setHintingPreference( QFont::HintingPreference( obj->getSInt32(  "hintingPrefrence" ) ) );

  return result;
}

template <>
QFont ConfigSection::getOrCreateValue( const QString key, const QFont defaultValue )
{ return ConfigSection::_getOrCreateValue( key, defaultValue ); }

#include <QPen>

template<>
JSONValue* ConfigSection::createValue( const QPen v ) const
{
  JSONObject* obj = new JSONObject();
  obj->insert( "color", ConfigSection::createValue( v.color() ) );
  obj->insert( "width", ConfigSection::createValue( v.widthF() ) );
  obj->insert( "style", ConfigSection::createValue<int>( v.style() ) );
  return obj;
}

template<>
QPen ConfigSection::getValue( const JSONValue* entry ) const
{
  QPen v;
  const JSONObject* obj = entry->cast<JSONObject>();
  if ( obj->has( "color" ) ) v.setColor( getValue<QColor>( obj->get( "color" ) ) );
  if ( obj->has( "width" ) ) v.setWidthF( getValue<double>( obj->get( "width" ) ) );
  if ( obj->has( "style" ) ) v.setStyle( Qt::PenStyle( getValue<int>( obj->get( "style" ) ) ) );
  return v;
}

template <>
QPen ConfigSection::getOrCreateValue( const QString key, const QPen defaultValue )
{ return ConfigSection::_getOrCreateValue( key, defaultValue ); }

#include <QPointF>

template<>
JSONValue* ConfigSection::createValue( const QPointF v ) const
{
  JSONObject* obj = new JSONObject();
  obj->insert( "x", createValue( v.x() ) );
  obj->insert( "y", createValue( v.y() ) );
  return obj;
}

template<>
QPointF ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  return QPointF(
    obj->getFloat64( "x" ),
    obj->getFloat64( "y" )
  );
}

template <>
QPointF ConfigSection::getOrCreateValue( const QString key, const QPointF defaultValue )
{ return ConfigSection::_getOrCreateValue( key, defaultValue ); }