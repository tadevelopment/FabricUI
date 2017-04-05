// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "ItemView.h"
#include "HeatBar.h"
#include "Data.h"

#include <QLayout>
#include <QLabel>
#include <QPainter>
#include <QVariant>
#include <QPushButton>

using namespace FabricUI::DFG::TabSearch;

void FabricUI::DFG::TabSearch::SetWidgetHighlight( QWidget* w, const bool highlighted )
{
  w->setProperty( "highlighted", QVariant( highlighted ) );
  w->setStyleSheet( w->styleSheet() );
}

inline std::string FormattedCat( const Query::Tag& tag )
{
  // Capitalize the name of the category
  std::string category = tag.cat();
  if( category.size() > 0 )
    category[0] = toupper( category[0] );
  return category;
}

std::string TagWidget::DisplayName( const Query::Tag& tag )
{
  const std::string category = FormattedCat( tag );
  return category + ':' + std::string(tag.name());
}

TagWidget::TagWidget( const Query::Tag& tag )
  : m_tag( tag )
  , m_hovered( false )
  , m_highlighted( false )
{
  this->setObjectName( "TagWidget" );

  QHBoxLayout* lay = new QHBoxLayout();
  lay->setMargin( 0 );
  m_button = new QPushButton( ToQString( tag.name() ) );
  lay->addWidget( m_button );
  this->setLayout( lay );
  m_button->setFocusPolicy( Qt::NoFocus );

  this->setToolTip( ToQString( FormattedCat( tag ) ) );
  this->setProperty( "tagCat", ToQString( tag.cat() ) );

  connect(
    m_button, SIGNAL( released() ),
    this, SLOT( onActivated() )
  );
  this->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );
  setHighlighted( false );
}

void TagWidget::setHighlighted( bool highlighted )
{
  SetWidgetHighlight( this, highlighted );
}

void TagWidget::onActivated()
{
  emit activated( m_tag );
}

void TagWidget::setScore( double score )
{
  //this->setToolTip( "Score = " + QString::number( score ) );
}

void TagWidget::enterEvent( QEvent* e )
{
  Parent::enterEvent( e );
  if( !m_isDisabled )
    this->setCursor( Qt::PointingHandCursor );
}

void TagWidget::leaveEvent( QEvent* e )
{
  Parent::leaveEvent( e );
  this->unsetCursor();
}

void TagWidget::connectToQuery( const Query& query )
{
  connect(
    &query, SIGNAL( changed( const Query& ) ),
    this, SLOT( onQueryChanged( const Query& ) )
  );
  onQueryChanged( query );
}

void Label::connectToQuery( const Query& query )
{
  connect(
    &query, SIGNAL( changed( const Query& ) ),
    this, SLOT( onQueryChanged( const Query& ) )
  );
  onQueryChanged( query );
}

void TagWidget::onQueryChanged( const Query& query )
{
  m_button->setDisabled( query.hasTag( m_tag ) );
  m_isDisabled = query.hasTag( m_tag );
  this->setProperty( "used", query.hasTag(m_tag) );
  this->setStyleSheet( this->styleSheet() );
}

void Label::onQueryChanged( const Query& query )
{
  m_isDisabled = query.hasTag( m_tag );
  this->setProperty( "used", this->m_isTag && query.hasTag( m_tag ) );
  this->setStyleSheet( this->styleSheet() );
}

size_t NameSep( const Result& result )
{
  return result.isPreset() ?
    result.rfind( '.' ) :
    result.type().size()
  ;
}

std::string PresetView::DisplayName( const Result& result )
{
  return result.substr( NameSep( result ) + 1 );
}

PresetView::PresetView( const Result& presetName, const std::vector<Query::Tag>& tags )
  : m_result( presetName )
  , m_heatBar( new HeatBar( this ) )
{
  this->setObjectName( "PresetView" );

  size_t dotI = NameSep( presetName );
  std::string baseName = presetName.substr( dotI+1 );
  std::string path = presetName.substr( 0, dotI );
  QHBoxLayout* lay = new QHBoxLayout();
  lay->setSpacing( 8 );
  lay->setMargin( 0 );
  lay->setContentsMargins( QMargins( 0, 0, 8, 0 ) );
  this->setLayout( lay );
  QLabel* nameLabel = new QLabel( "<b>" + ToQString( baseName ) + "</b>" );
  this->layout()->addWidget( nameLabel );
  this->layout()->setAlignment( nameLabel, Qt::AlignLeft );
  nameLabel->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );

  for( size_t i = 0; i < tags.size(); i++ )
  {
    TagWidget* tagWidget = new TagWidget( tags[i] );
    connect(
      tagWidget, SIGNAL( activated( const Query::Tag& ) ),
      this, SIGNAL( requestTag( const Query::Tag& ) )
    );
    this->m_tagWidgets.push_back( tagWidget );
    this->layout()->addWidget( tagWidget );
  }
  this->layout()->addWidget( m_heatBar );
  setHighlighted( false );
}

void PresetView::setHighlighted( bool highlighted )
{
  SetWidgetHighlight( this, highlighted );
  for( size_t i = 0; i < m_tagWidgets.size(); i++ )
    m_tagWidgets[i]->setHighlighted( highlighted );
}

void PresetView::setScore( double score, double minScore, double maxScore )
{
  m_heatBar->set( score, minScore, maxScore );
}

void Label::set( const std::string& text )
{
  m_isTag = false;
  this->setProperty( "clickable", m_isTag );
  this->setStyleSheet( this->styleSheet() );
  m_tag = Query::Tag();
  this->setText( ToQString( text ) );
}

void Label::set( const std::string& text, const Query::Tag& tag )
{
  m_isTag = true;
  this->setProperty( "clickable", m_isTag );
  this->setStyleSheet( this->styleSheet() );
  m_tag = tag;
  this->setText( ToQString( text ) );
}

void Label::mouseReleaseEvent( QMouseEvent * e )
{
  Parent::mouseReleaseEvent( e );
  if( m_isTag )
    emit requestTag( m_tag );
}

void Label::init()
{
  this->setObjectName( "TabSearchLabel" );
}

void Label::enterEvent( QEvent* e )
{
  Parent::enterEvent( e );
  if( m_isTag && !m_isDisabled )
    this->setCursor( Qt::PointingHandCursor );
}

void Label::leaveEvent( QEvent* e )
{
  Parent::leaveEvent( e );
  if( m_isTag )
    this->unsetCursor();
}
