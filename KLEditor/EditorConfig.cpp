// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "EditorConfig.h"

#include <FabricUI/Util/Config.h>

using namespace FabricUI::KLEditor;
using namespace FTL;

namespace FabricUI {
namespace Util {

// TODO : move it to Config.cpp if it's useful somewhere else
template<>
JSONValue* ConfigSection::createValue( const QTextCharFormat v ) const
{
  JSONObject* obj = new JSONObject();

  if( v.hasProperty( QTextCharFormat::ForegroundBrush ) )
    obj->insert( "foreground", createValue( v.foreground().color() ) );

  if( v.hasProperty( QTextCharFormat::FontUnderline ))
    obj->insert( "fontUnderline", createValue( v.fontUnderline() ) );

  if( v.hasProperty( QTextCharFormat::TextUnderlineColor ) )
    obj->insert( "underlineColor", createValue( v.underlineColor() ) );

  if( v.hasProperty( QTextCharFormat::TextUnderlineStyle ) )
    obj->insert( "underlineStyle", createValue( int(v.underlineStyle()) ) );

  if( v.hasProperty( QTextCharFormat::BackgroundBrush ) )
    obj->insert( "background", createValue( v.background().color() ) );

  if( v.hasProperty( QTextFormat::FullWidthSelection ) )
    obj->insert( "fullWidthSelection", createValue( v.property( QTextCharFormat::FullWidthSelection ).value<bool>() ) );

  return obj;
}

template<>
QTextCharFormat ConfigSection::getValue( const JSONValue* entry ) const
{
  const JSONObject* obj = entry->cast<JSONObject>();
  QTextCharFormat v;

  if( obj->has( "foreground" ) )
    v.setForeground( getValue<QColor>( obj->get( "foreground" ) ) );

  if( obj->has( "fontUnderline" ))
    v.setFontUnderline( obj->getBoolean( "fontUnderline" ) );

  if( obj->has( "underlineColor" ) )
    v.setUnderlineColor( getValue<QColor>( obj->get( "underlineColor" ) ) );

  if( obj->has( "underlineStyle" ) )
    v.setUnderlineStyle( QTextCharFormat::UnderlineStyle( getValue<int>( obj->get( "underlineStyle" ) ) ) );

  if( obj->has( "background" ))
    v.setBackground( getValue<QColor>( obj->get( "background" ) ) );

  if( obj->has( "fullWidthSelection" ) )
    v.setProperty( QTextCharFormat::FullWidthSelection, getValue<bool>( obj->get( "fullWidthSelection" ) ) );

  return v;
}

}} // namespace FabricUI::Util

EditorConfig::EditorConfig()
{
  Util::Config rootConfig;
  Util::ConfigSection& cfg = rootConfig.getOrCreateSection( "KLEditor" );

#define GET_PARAMETER( parameter, defaultValue ) \
  parameter = cfg.getOrCreateValue( #parameter, defaultValue )

  GET_PARAMETER( codeBackgroundColor, QColor(39, 40, 34) );
  GET_PARAMETER( codeFontSize, 10 );
  GET_PARAMETER( codeFont, QFont("Roboto Mono", codeFontSize) );
  GET_PARAMETER( codeFontColor, QColor(248, 248, 242) );
  GET_PARAMETER( codeTabWidth, 2 );

  GET_PARAMETER( lineNumberFontSize, codeFontSize );
  GET_PARAMETER( lineNumberFont, codeFont );
  GET_PARAMETER( lineNumberBackgroundColor, QColor(39, 40, 34) );
  GET_PARAMETER( lineNumberFontColor, QColor(139, 140, 135) );

  GET_PARAMETER( codeCompletionFontSize, codeFontSize );
  GET_PARAMETER( codeCompletionFont, codeFont );
  GET_PARAMETER( codeCompletionBackgroundColor, QColor(236, 244, 249) );
  GET_PARAMETER( codeCompletionActiveBackgroundColor, QColor(92, 172, 248) );
  GET_PARAMETER( codeCompletionFontColor, QColor(0, 0, 0) );
  GET_PARAMETER( codeCompletionFontColorSuffix, QColor(80, 80, 80) );
  GET_PARAMETER( codeCompletionMargins, 1 );
  GET_PARAMETER( codeCompletionSpacing, 0 );

#define GET_TEXT_CHAR_FORMAT( format ) \
  GET_PARAMETER( format, format )

  formatForComment.setForeground(QColor( 117, 113, 94));
  GET_TEXT_CHAR_FORMAT( formatForComment );

  formatForString.setForeground(Qt::yellow);
  GET_TEXT_CHAR_FORMAT( formatForString );

  formatForNumber.setForeground(QColor(147, 112, 219));
  GET_TEXT_CHAR_FORMAT( formatForNumber );

  formatForKeyword.setForeground( QColor(255, 0, 127) );
  GET_TEXT_CHAR_FORMAT( formatForKeyword );

  formatForType.setForeground(Qt::cyan);
  GET_TEXT_CHAR_FORMAT( formatForType );

  formatForConstant.setForeground(Qt::green);
  GET_TEXT_CHAR_FORMAT( formatForConstant );

  formatForMethod.setForeground(Qt::cyan);
  GET_TEXT_CHAR_FORMAT( formatForMethod );
  // formatForError.setBackground(QColor(255, 0, 0, 80));

  formatForError.setFontUnderline(true);
  formatForError.setUnderlineColor(QColor(255, 102, 0, 255));
  formatForError.setUnderlineStyle(QTextCharFormat::WaveUnderline);
  GET_TEXT_CHAR_FORMAT( formatForError );

  formatForHighlight.setBackground(QColor(230, 219, 116, 50));
  formatForHighlight.setProperty(QTextFormat::FullWidthSelection, true);
  GET_TEXT_CHAR_FORMAT( formatForHighlight );

  GET_PARAMETER( editorAutoRebuildAST, true );
  GET_PARAMETER( editorAlwaysShowCodeCompletion, true );
  GET_PARAMETER( editorSpacing, 2 );

  GET_PARAMETER( docUrlPrefix, QString("http://documentation.fabricengine.com/FabricEngine/latest/HTML/") );
}
