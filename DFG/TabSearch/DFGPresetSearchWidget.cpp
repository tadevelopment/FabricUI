// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DFGPresetSearchWidget.h"

#include <FTL/JSONValue.h>
#include <QLayout>

using namespace FabricUI::DFG;

DFGPresetSearchWidget::DFGPresetSearchWidget( FabricCore::DFGHost* host )
  : m_clearQueryOnClose( false )
  , m_host( host )
  , m_resultPreview( NULL )
{
  this->setWindowFlags( Qt::Popup );

  QVBoxLayout* vlayout = new QVBoxLayout();

  m_queryEdit = new TabSearch::QueryEdit( m_host );
  m_queryEdit->setMinimumWidth( 800 );
  vlayout->addWidget( m_queryEdit );
  connect(
    m_queryEdit, SIGNAL( queryChanged( const TabSearch::Query& ) ),
    this, SLOT( onQueryChanged( const TabSearch::Query& ) )
  );

  m_resultsView = new TabSearch::ResultsView();
  m_resultsView->setFocusProxy( this );
  vlayout->addWidget( m_resultsView );
  connect(
    m_resultsView, SIGNAL( presetSelected( QString ) ),
    this, SLOT( setPreview( QString ) )
  );
  connect(
    m_resultsView, SIGNAL( presetValidated( QString ) ),
    this, SIGNAL( selectedPreset( QString ) )
  );
  connect(
    m_resultsView, SIGNAL( presetValidated( QString ) ),
    this, SLOT( close() )
  );
  connect(
    m_resultsView, SIGNAL( tagRequested( const std::string& ) ),
    m_queryEdit, SLOT( requestTag( const std::string& ) )
  );
  connect(
    m_resultsView, SIGNAL( tagsRequested( const std::vector<std::string>& ) ),
    m_queryEdit, SLOT( requestTags( const std::vector<std::string>& ) )
  );
  connect(
    m_resultsView, SIGNAL( presetDeselected() ),
    this, SLOT( hidePreview() )
  );

  // Selecting elements (the selection must be exclusive to either
  // the ResultsView or the QueryEdit
  connect(
    m_resultsView, SIGNAL( selectingItems() ),
    m_queryEdit, SLOT( deselectTags() )
  );
  connect(
    m_queryEdit, SIGNAL( selectingTags() ),
    m_resultsView, SLOT( clearSelection() )
  );

  vlayout->setMargin( 0 );
  vlayout->setSpacing( 0 );

  QHBoxLayout* hlayout = new QHBoxLayout();
  hlayout->addLayout( vlayout );
  hlayout->setMargin( 0 );

  this->setLayout( hlayout );
}

void DFGPresetSearchWidget::showForSearch( QPoint globalPos )
{
  move( QPoint( 0, 0 ) );
  move( mapFromGlobal( globalPos ) );

  emit enabled( true );
  show();
  m_queryEdit->setFocus();
}

void DFGPresetSearchWidget::keyPressEvent( QKeyEvent *event )
{
  switch( event->key() )
  {
    case Qt::Key_Enter :
    case Qt::Key_Return :
      validateSelection(); break;
    case Qt::Key_Up :
    case Qt::Key_Down :
      m_resultsView->keyPressEvent( event ); break;
    case Qt::Key_Tab :
      close();
    default:
      Parent::keyPressEvent( event );
  }
}

void DFGPresetSearchWidget::onQueryChanged( const TabSearch::Query& query )
{
  // Splitting the search string into a char**
  const std::string searchStr = query.getText();

  std::vector<std::string> searchTermsStr = query.getSplitText();
  std::vector<char const*> searchTerms( searchTermsStr.size() );

  for( unsigned int i = 0; i < searchTermsStr.size(); i++ )
    searchTerms[i] = searchTermsStr[i].data();

  std::vector<char const *> requiredTags;
  const TabSearch::Query::Tags& queryTags = query.getTags();
  requiredTags.resize( queryTags.size() );
  for( size_t i=0; i<queryTags.size(); i++ )
    requiredTags[i] = queryTags[i].data();

  // Querying the DataBase of presets
  FabricCore::DFGHost* host = m_host;
  FEC_StringRef jsonStr = FEC_DFGHostSearchPresets(
    host->getFECDFGHostRef(),
    searchTerms.size(),
    searchTerms.data(),
    requiredTags.size(),
    requiredTags.data(),
    0,
    16
  );
  FTL::StrRef jsonStrR( FEC_StringGetCStr( jsonStr ), FEC_StringGetSize( jsonStr ) );

  hidePreview();
  m_resultsView->setResults( jsonStrR, query );

  { // HACK
    m_resultsView->setMinimumHeight( m_resultsView->sizeHint().height() );
    adjustSize();
    adjustSize();
  }
}

void DFGPresetSearchWidget::validateSelection()
{
  m_resultsView->validateSelection();
  m_queryEdit->removeHighlightedTag();
}

void DFGPresetSearchWidget::hideEvent( QHideEvent* e )
{
  Parent::hideEvent( e );
  this->close();
}

bool DFGPresetSearchWidget::focusNextPrevChild( bool next )
{
  return false;
}

void DFGPresetSearchWidget::hidePreview()
{
  if( m_resultPreview != NULL )
  {
    layout()->removeWidget( m_resultPreview );
    m_resultPreview->deleteLater();
    m_resultPreview = NULL;
  }
}

void DFGPresetSearchWidget::setPreview( QString preset )
{
  //if( m_resultPreview == NULL || preset != m_resultPreview->getPreset() )
  if( false ) // HACK : Disabled the Preview
  {
    this->hidePreview();
    m_resultPreview = new TabSearch::ResultPreview( preset, m_host );
    layout()->addWidget( m_resultPreview );
    connect(
      m_resultPreview, SIGNAL( tagRequested( const std::string& ) ),
      m_queryEdit, SLOT( requestTag( const std::string& ) )
    );
  }
}

void DFGPresetSearchWidget::close()
{
  if( m_clearQueryOnClose )
    m_queryEdit->clear();
  else
    // Selecting all the query : the user will only 
    // need to press Backspace to clear the query
    m_queryEdit->selectAll();

  emit enabled( false );
  if( !this->isHidden() )
    this->hide();
}
