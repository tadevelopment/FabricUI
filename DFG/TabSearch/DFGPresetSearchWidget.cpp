// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DFGPresetSearchWidget.h"

#include <FTL/JSONValue.h>
#include <QLayout>

using namespace FabricUI::DFG;

DFGPresetSearchWidget::DFGPresetSearchWidget( FabricCore::DFGHost* host )
  : m_host( host )
  , m_resultPreview( NULL )
{
  this->setWindowFlags( Qt::Popup );

  QVBoxLayout* vlayout = new QVBoxLayout();

  m_queryEdit = new TabSearch::QueryEdit();
  m_queryEdit->setMinimumWidth( 800 );
  vlayout->addWidget( m_queryEdit );
  connect( m_queryEdit, SIGNAL( queryChanged( QString ) ),
    this, SLOT( onQueryChanged( QString ) ) );

  m_resultsView = new TabSearch::ResultsView();
  vlayout->addWidget( m_resultsView );
  connect(
    m_resultsView, SIGNAL( presetSelected( QString ) ),
    this, SLOT( setPreview( QString ) )
  );

  vlayout->setMargin( 0 );
  vlayout->setSpacing( 0 );

  QHBoxLayout* hlayout = new QHBoxLayout();
  hlayout->addLayout( vlayout );
  hlayout->setMargin( 0 );

  this->setLayout( hlayout );
  m_queryEdit->setFocus();
}

void DFGPresetSearchWidget::showForSearch( QPoint globalPos )
{
  move( QPoint( 0, 0 ) );
  move( mapFromGlobal( globalPos ) );

  emit enabled( true );
  show();
}

void DFGPresetSearchWidget::keyPressEvent( QKeyEvent *event )
{
  switch( event->key() )
  {
    case Qt::Key_Enter :
    case Qt::Key_Return :
      validateSelection(); break;
    case Qt::Key_Up :
      m_resultsView->moveSelection( -1 ); break;
    case Qt::Key_Down :
      m_resultsView->moveSelection( +1 ); break;
    default:
      Parent::keyPressEvent( event );
  }
}

void DFGPresetSearchWidget::onQueryChanged( QString query )
{
  this->hidePreview();

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
  close();
}

void DFGPresetSearchWidget::hideEvent( QHideEvent* e )
{
  Parent::hideEvent( e );
  this->close();
}

bool DFGPresetSearchWidget::focusNextPrevChild( bool next )
{
  this->close();
  return false;
}

void DFGPresetSearchWidget::hidePreview()
{
  if( m_resultPreview != NULL )
  {
    layout()->removeWidget( m_resultPreview );
    delete m_resultPreview;
    m_resultPreview = NULL;
  }
}

void DFGPresetSearchWidget::setPreview( QString preset )
{
  this->hidePreview();
  m_resultPreview = new TabSearch::ResultPreview( preset, m_host );
  layout()->addWidget( m_resultPreview );
}

void DFGPresetSearchWidget::close()
{
  m_queryEdit->setText( "" );
  emit enabled( false );
  if( !this->isHidden() )
    this->hide();
}
