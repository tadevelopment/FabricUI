// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultPreview.h"

#include <FabricCore.h>
#include <iostream>
#include <QLayout>
#include <QLabel>
#include <FTL/JSONValue.h>

using namespace FabricUI::DFG::TabSearch;
using namespace FabricCore;

ResultPreview::ResultPreview( QString preset, DFGHost* host )
  : m_preset( preset )
{
  QVBoxLayout* layout = new QVBoxLayout();

  String descStr = host->getPresetDesc( preset.toStdString().data() );
  FTL::JSONValue* desc = FTL::JSONValue::Decode( std::string( descStr.getCStr(), descStr.getSize() ) );
  FTL::CStrRef nameStr = desc->cast<FTL::JSONObject>()->getString( "presetName" );

  QFont font;

  // Name
  {
    QLabel* label = new QLabel( "<b>" + QString::fromStdString( nameStr ) + "</b>" );
    font.setPointSize( 16 ); label->setFont( font );
    layout->addWidget( label );
  }

  // Path
  {
    QLabel* label = new QLabel( "<i>" + preset + "</i>" );
    font.setPointSize( 8 ); label->setFont( font );
    layout->addWidget( label );
  }

  // Tags
#if 1 // Hack : we should use a Core/External service instead of parsing the DB here
  try
  {
    String dbStrR = host->dumpPresetSearchDB();
    FTL::StrRef dbStr( dbStrR.getCStr(), dbStrR.getSize() );
    FTL::JSONValue* db = FTL::JSONValue::Decode( dbStr );
    FTL::JSONObject* dbO = db->cast<FTL::JSONObject>();

    std::string presetName = preset.toStdString();

    for( FTL::JSONObject::const_iterator it=dbO->begin(); it!=dbO->end(); it++ )
    {
      const FTL::JSONObject* tagV = it->second->cast<FTL::JSONObject>();
      const FTL::JSONArray* presets = tagV->getArray( "presets" );
      for( size_t pI = 0; pI < presets->size(); pI++ )
      {
        const FTL::JSONArray* preset = presets->getArray( pI );
        if( presetName == preset->getString( 0 ) )
        {
          QLabel* label = new QLabel( QString::fromStdString(it->first) );
          label->setToolTip(
            "TagWeight : " + QString::number( tagV->getFloat64( "weight" ) )
            + "; TagAndPresetWeight : " + QString::number( preset->getFloat64( 1 ) )
          );
          layout->addWidget( label );
        }
      }
    }
  }
  catch( FTL::JSONException e )
  {
    std::cout << e.getDesc() << std::endl;
  }
#endif

  layout->setAlignment( Qt::AlignTop );
  this->setLayout( layout );
}

QString ResultPreview::getPreset() const { return m_preset; }
