// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DFGPresetSearchWidget.h"

#include <FTL/JSONValue.h>
#include <QLayout>

using namespace FabricUI::DFG;

DFGPresetSearchWidget::DFGPresetSearchWidget( FabricCore::DFGHost* host )
  : m_host( host )
{
  this->setWindowFlags( Qt::Popup );

  QVBoxLayout* layout = new QVBoxLayout();

  m_queryEdit = new TabSearch::QueryEdit();
  layout->addWidget( m_queryEdit );
  connect( m_queryEdit, SIGNAL( queryChanged( QString ) ),
    this, SLOT( onQueryChanged( QString ) ) );

  m_resultsView = new TabSearch::ResultsView();
  layout->addWidget( m_resultsView );

  layout->setMargin( 0 );
  layout->setSpacing( 0 );
  this->setLayout( layout );
  m_queryEdit->setFocus();
}

void DFGPresetSearchWidget::showForSearch( QPoint globalPos )
{
  move( mapFromGlobal( globalPos ) );

  emit enabled( true );
  show();
}

void DFGPresetSearchWidget::keyPressEvent( QKeyEvent *event )
{
  if( event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return )
    validateSelection();
  else
  if( event->key() == Qt::Key_Up )
    m_resultsView->moveSelection( -1 );
  else
  if( event->key() == Qt::Key_Down )
    m_resultsView->moveSelection( +1 );
  else
    Parent::keyPressEvent( event );
}

void DFGPresetSearchWidget::onQueryChanged( QString query )
{
  // Splitting the search string into a char**
  const std::string searchStr = query.toStdString().data();

  std::vector<std::string> tagsStr;
  unsigned int start = 0;
  for( unsigned int end = 0; end < searchStr.size(); end++ )
  {
    const char c = searchStr[end];
    if( c == '.' || c == ' ' ) // delimiters
    {
      if( end - start > 0 )
        tagsStr.push_back( searchStr.substr( start, end - start ) );
      start = end + 1;
    }
  }
  if( start < searchStr.size() )
    tagsStr.push_back( searchStr.substr( start, searchStr.size() - start ) );

  std::vector<char const*> tags( tagsStr.size() );

  // Debug : TODO remove
  /*
  for( unsigned int i = 0; i < tagsStr.size(); i++ )
    std::cout << "\"" << tagsStr[i] << "\" ";
  std::cout << std::endl;
  */

  for( unsigned int i = 0; i < tagsStr.size(); i++ )
    tags[i] = tagsStr[i].data();

  // Querying the DataBase of presets
  FabricCore::DFGHost* host = m_host;
  FEC_StringRef jsonStr = FEC_DFGHostSearchPresets(
    host->getFECDFGHostRef(),
    tags.size(),
    tags.data(),
    0,
    16
  );
  FTL::StrRef jsonStrR( FEC_StringGetCStr( jsonStr ), FEC_StringGetSize( jsonStr ) );
  const FTL::JSONValue* json = FTL::JSONValue::Decode( jsonStrR );
  const FTL::JSONObject* root = json->cast<FTL::JSONObject>();
  const FTL::JSONArray* resultsJson = root->getArray( "results" );

  std::vector<std::string> results;
  for( unsigned int i = 0; i < resultsJson->size(); i++ )
    results.push_back( resultsJson->getArray( i )->getString( 0 ) );

  m_resultsView->setResults( results );
}

void DFGPresetSearchWidget::validateSelection()
{
  if( m_resultsView->numberResults() )
    emit selectedPreset( m_resultsView->getSelectedPreset() );
  std::cout << "Selected " << m_resultsView->getSelectedPreset().toStdString() << std::endl;
}
