// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ItemView.h"

#include <QLayout>

using namespace FabricUI::DFG::TabSearch;

TagView::TagView( const std::string& tagName )
  : QPushButton( QString::fromStdString( tagName ) )
{
  this->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );
  this->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
  this->setStyleSheet( "background-color: rgba( 50, 30, 0, 50 );" );
}

void TagView::setScore( double score )
{
  this->setToolTip( "Score = " + QString::number( score ) );
}

PresetView::PresetView( const std::string& presetName )
  : m_scoreLabel( NULL )
{
  size_t dotI = presetName.rfind( '.' );
  std::string baseName = presetName.substr( dotI+1 );
  std::string path = presetName.substr( 0, dotI );
  QHBoxLayout* lay = new QHBoxLayout();
  lay->setSpacing( 8 );
  lay->setMargin( 0 );
  lay->setContentsMargins( QMargins( 0, 0, 32, 0 ) );
  this->setLayout( lay );
  QLabel* nameLabel = new QLabel( "<b>" + QString::fromStdString( baseName ) + "</b>" );
  this->layout()->addWidget( nameLabel );
  nameLabel->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );
  QLabel* pathLabel = new QLabel( "<i>" + QString::fromStdString( path ) + "</i>" );
  this->layout()->addWidget( pathLabel );
}

void PresetView::setScore( double score )
{
  //this->setToolTip( "Score = " + QString::number( score ) );
  if( m_scoreLabel == NULL )
  {
    m_scoreLabel = new QLabel();
    this->layout()->addWidget( m_scoreLabel );
    this->layout()->setAlignment( m_scoreLabel, Qt::AlignRight );
  }
  m_scoreLabel->setText( QString::number( score ) );
}
