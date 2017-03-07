// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ItemView.h"
#include "HeatBar.h"

#include <QLayout>
#include <QLabel>
#include <QPushButton>

using namespace FabricUI::DFG::TabSearch;

TagView::TagView( const std::string& tagName )
  : m_name( tagName )
  , m_hovered( false )
  , m_highlighted( false )
{
  QHBoxLayout* lay = new QHBoxLayout();
  lay->setMargin( 0 );
  m_button = new QPushButton( QString::fromStdString( tagName ) );
  m_button->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );
  m_button->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
  lay->addWidget( m_button );
  this->setLayout( lay );
  m_button->setFocusPolicy( Qt::NoFocus );

  connect(
    m_button, SIGNAL( released() ),
    this, SLOT( onActivated() )
  );
  this->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );
  this->updateHighlightColor();
}

void TagView::setHighlighted( bool highlighted )
{
  m_highlighted = highlighted;
  updateHighlightColor();
}

void TagView::updateHighlightColor()
{
  m_button->setStyleSheet(
    ( m_highlighted || m_hovered ) ?
    "color : #000; background-color: rgba( 255, 255, 255, 255 );" :
    "color: #FFF; background-color: rgba( 50, 30, 0, 50 );"
  );
}

void TagView::enterEvent( QEvent * e )
{
  Parent::enterEvent( e );
  m_hovered = true;
  updateHighlightColor();
}

void TagView::leaveEvent( QEvent * e )
{
  Parent::leaveEvent( e );
  m_hovered = false;
  updateHighlightColor();
}

void TagView::onActivated()
{
  emit activated( m_name );
}

void TagView::setScore( double score )
{
  this->setToolTip( "Score = " + QString::number( score ) );
}

PresetView::PresetView( const std::string& presetName, const std::vector<std::string>& tags )
  : m_heatBar( new HeatBar( this ) )
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
  //QLabel* pathLabel = new QLabel( "<i>" + QString::fromStdString( path ) + "</i>" );
  //this->layout()->addWidget( pathLabel );
  for( size_t i = 0; i < tags.size(); i++ )
  {
    TagView* tagView = new TagView( tags[i] );
    this->m_tagViews.push_back( tagView );
    this->layout()->addWidget( tagView );
  }
  this->layout()->addWidget( m_heatBar );
  this->layout()->setAlignment( m_heatBar, Qt::AlignRight | Qt::AlignVCenter );
  setHighlighted( false );
}

void PresetView::setHighlighted( bool highlighted )
{
  setStyleSheet( highlighted ? "color : #000;" : "color: #FFF;" );
  for( size_t i = 0; i < m_tagViews.size(); i++ )
    m_tagViews[i]->setHighlighted( highlighted );
}

void PresetView::setScore( double score, double minScore, double maxScore )
{
  m_heatBar->set( score, minScore, maxScore );
  m_heatBar->setToolTip( QString::number( score ) );
  m_heatBar->setStyleSheet( "color: #000;" );
}
