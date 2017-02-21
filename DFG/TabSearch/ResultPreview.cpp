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

  layout->setAlignment( Qt::AlignTop );
  this->setLayout( layout );
}
