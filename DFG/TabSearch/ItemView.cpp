// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ItemView.h"

#include <QLayout>
#include <QLabel>
#include <QPushButton>

using namespace FabricUI::DFG::TabSearch;

TagView::TagView( const std::string& tagName )
  : m_name( QString::fromStdString( tagName ) )
{
  QHBoxLayout* lay = new QHBoxLayout();
  lay->setMargin( 0 );
  QPushButton* button = new QPushButton( QString::fromStdString( tagName ) );
  button->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );
  button->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
  button->setStyleSheet( "background-color: rgba( 50, 30, 0, 50 );" );
  lay->addWidget( button );
  this->setLayout( lay );

  connect(
    button, SIGNAL( released() ),
    this, SLOT( onActivated() )
  );
  this->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );
}

void TagView::onActivated()
{
  emit activated( m_name );
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
