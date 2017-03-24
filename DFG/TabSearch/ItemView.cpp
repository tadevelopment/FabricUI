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

std::string TagWidget::DisplayName( const Query::Tag& result )
{
  Query::Tag tag = result;
  // Capitalize the name of the category
  std::string category = tag.cat();
  if( category.size() > 0 )
    category[0] = toupper( category[0] );
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
  m_button = new QPushButton( ToQString( DisplayName( tag ) ) );
  lay->addWidget( m_button );
  this->setLayout( lay );
  m_button->setFocusPolicy( Qt::NoFocus );

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
  this->setToolTip( "Score = " + QString::number( score ) );
}

TagArrow::TagArrow()
  : m_spacing(8)
  , m_leftHighlighted( false )
  , m_hasRight( false )
  , m_rightHighlighted( false )
  , m_bgColor(QColor(0,0,0))
  , m_highlightBgColor(QColor(255,255,255))
{
  this->setObjectName( "TagArrow" );
  this->setMinimumWidth( 14 );
}

void TagArrow::paintEvent( QPaintEvent *e )
{
  QRect r = this->contentsRect();
  r.setSize( QSize( r.width() + 1, r.height() + 1 ) );
  QPainter p( this );
  p.setRenderHint( QPainter::Antialiasing, true );
  {
    // Left triangle
    QPainterPath path;
    QPolygon poly;
    poly.append( r.topLeft() );
    poly.append( QPoint(
      r.right() - m_spacing,
      ( r.top() + r.bottom() ) / 2
    ) );
    poly.append( r.bottomLeft() );
    path.addPolygon( poly );
    p.fillPath( path, m_leftHighlighted ? m_highlightBgColor : m_bgColor );
  }
  for( size_t i = 0; m_hasRight && i<2; i++ )
  {
    // Right triangles
    QPainterPath path;
    QPolygon poly;
    poly.append( QPoint(
      r.left() + m_spacing,
      i == 0 ? r.top() : r.bottom()
    ) );
    poly.append( ( r.topRight() + r.bottomRight() ) / 2 );
    poly.append( i == 0 ? r.topRight() : r.bottomRight() );
    path.addPolygon( poly );
    p.fillPath( path, m_rightHighlighted ? m_highlightBgColor : m_bgColor );
  }
  Parent::paintEvent( e );
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
  : m_heatBar( new HeatBar( this ) )
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
  nameLabel->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum ) );
  //QLabel* pathLabel = new QLabel( "<i>" + ToQString( path ) + "</i>" );
  //this->layout()->addWidget( pathLabel );
  if( tags.size() > 0 )
  {
    QWidget* emptySpace = new QWidget();
    int separtorW = 32;
    emptySpace->setMinimumWidth( separtorW );
    emptySpace->setMaximumWidth( separtorW );
    lay->addWidget( emptySpace );
  }
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
  this->layout()->setAlignment( m_heatBar, Qt::AlignRight | Qt::AlignVCenter );
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
  m_heatBar->setToolTip( QString::number( score ) );
  m_heatBar->setStyleSheet( "color: #000;" );
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
  if( m_isTag )
    this->setCursor( Qt::PointingHandCursor );
}

void Label::leaveEvent( QEvent* e )
{
  Parent::leaveEvent( e );
  if( m_isTag )
    this->unsetCursor();
}
