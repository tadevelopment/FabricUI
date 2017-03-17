// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ResultPreview.h"

#include "ItemView.h"

#include <FabricCore.h>
#include <iostream>
#include <QLayout>
#include <QLabel>
#include <FTL/JSONValue.h>

using namespace FabricUI::DFG::TabSearch;
using namespace FabricCore;

ResultPreview::ResultPreview( FabricCore::DFGHost* host )
  : m_host( host )
  , m_name( new QLabel() )
{
  this->setObjectName( "ResultPreview" );
  m_name->setObjectName( "Name" );

  clear();
  QVBoxLayout* lay = new QVBoxLayout();
  lay->setMargin( 0 );
  lay->setAlignment( Qt::AlignTop );
  this->setLayout( lay );
  this->layout()->addWidget( m_name );
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
  m_name->setText( "<b>" + QString::fromStdString( name ) + "</b>" );
}

const std::string& ResultPreview::getPreset() const { return m_preset; }
