// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DFGPresetSearchWidget.h"

#include <FTL/JSONValue.h>
#include <QDebug>
#include <QFrame>
#include <QScrollArea>
#include <QAction>
#include <QScrollBar>
#include <QPushButton>
#include <QLabel>
#include <QLayout>

using namespace FabricUI::DFG;

DFGPresetSearchWidget::DFGPresetSearchWidget( FabricCore::DFGHost* host )
  : m_clearQueryOnClose( false )
  , m_staticEntriesAddedToDB( false )
  , m_host( host )
  , m_searchFrame( new QFrame() )
  , m_status( new QLabel() )
  , m_resultPreview( new TabSearch::ResultPreview( m_host ) )
  , m_detailsPanel( new QScrollArea() )
  , m_detailsPanelToggled( true )
{

  registerStaticEntries();

  this->setObjectName( "DFGPresetSearchWidget" );
  m_searchFrame->setObjectName( "SearchFrame" );
  m_detailsPanel->setObjectName( "DetailsPanel" );

  this->setWindowFlags( Qt::Popup );
  QVBoxLayout* vlayout = new QVBoxLayout();

  m_queryEdit = new TabSearch::QueryEdit( m_host );
  m_queryEdit->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed ) );
  vlayout->addWidget( m_queryEdit );
  connect(
    m_queryEdit, SIGNAL( queryChanged( const TabSearch::Query& ) ),
    this, SLOT( onQueryChanged( const TabSearch::Query& ) )
  );
  connect(
    m_queryEdit, SIGNAL( lostFocus() ),
    this, SLOT( close() )
  );

  m_resultsView = new TabSearch::ResultsView();
  m_resultsView->setFocusProxy( this );
  vlayout->addWidget( m_resultsView );
  connect(
    m_resultsView, SIGNAL( presetSelected( const std::string& ) ),
    this, SLOT( setPreview( const std::string& ) )
  );
  connect(
    m_resultsView, SIGNAL( presetValidated( const std::string& ) ),
    this, SLOT( onResultValidated( const std::string& ) )
  );
  connect(
    m_resultsView, SIGNAL( presetValidated( const std::string& ) ),
    this, SLOT( close() )
  );
  connect(
    m_resultsView, SIGNAL( presetValidated( const std::string& ) ),
    this, SIGNAL( giveFocusToParent() )
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
  vlayout->setSpacing( 4 );

  m_searchFrame->setLayout( vlayout );
  
  QHBoxLayout* hlayout = new QHBoxLayout();
  hlayout->setMargin( 0 ); hlayout->setSpacing( 0 );
  hlayout->addWidget( m_searchFrame );
  this->setLayout( hlayout );
  m_detailsPanel->setFocusPolicy( Qt::NoFocus );
  m_detailsPanel->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  QKeySequence toggleDetailsKey = Qt::CTRL + Qt::Key_Tab;
  {
    m_toggleDetailsButton = new TabSearch::Toggle();
    m_toggleDetailsButton->setObjectName( "ToggleDetailsPanelButton" );
    m_toggleDetailsButton->setToolTip( toggleDetailsKey.toString() );
    m_toggleDetailsButton->setFocusPolicy( Qt::NoFocus );
    QVBoxLayout* lay = new QVBoxLayout();
    lay->setMargin( 0 );
    m_toggleDetailsButton->setLayout( lay );
    QFrame* handle = new QFrame();
    handle->setObjectName( "Handle" );
    m_toggleDetailsButton->layout()->addWidget( handle );
    m_toggleDetailsButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Minimum ) );
    m_toggleDetailsButton->setToggled( m_detailsPanelToggled );
    connect(
      m_toggleDetailsButton, SIGNAL( toggled( bool ) ),
      this, SLOT( toggleDetailsPanel( bool ) )
    );
    hlayout->addWidget( m_toggleDetailsButton );
  }

  hlayout->addWidget( m_detailsPanel );

  m_detailsPanel->setWidget( m_resultPreview );
  connect(
    m_resultPreview, SIGNAL( tagRequested( const std::string& ) ),
    m_queryEdit, SLOT( requestTag( const std::string& ) )
  );

  // Close action
  {
    QAction* closeAction = new QAction( this );
    QList<QKeySequence> shortcuts;
    shortcuts.push_back( Qt::Key_Tab );
    shortcuts.push_back( Qt::Key_Escape );
    closeAction->setShortcuts( shortcuts );
    connect( closeAction, SIGNAL( triggered( bool ) ),
      this, SIGNAL( giveFocusToParent() ) );
    connect( closeAction, SIGNAL( triggered( bool ) ),
      this, SLOT( close() ) );
    this->addAction( closeAction );
  }

  // Toggle Details
  {
    QAction* toggleDetailsA = new QAction( this );
    toggleDetailsA->setShortcut( toggleDetailsKey );
    connect( toggleDetailsA, SIGNAL( triggered( bool ) ),
      this, SLOT( toggleDetailsPanel() ) );
    this->addAction( toggleDetailsA );
  }

  m_status->setObjectName( "Status" );
  vlayout->addWidget( m_status );
  hidePreview();
  updateSize();
}

void DFGPresetSearchWidget::showForSearch( QPoint globalPos )
{
  move( QPoint( 0, 0 ) );
  m_posAtShow = mapFromGlobal( globalPos );
  move( m_posAtShow );

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
    default:
      Parent::keyPressEvent( event );
  }
}

void DFGPresetSearchWidget::onQueryChanged( const TabSearch::Query& query )
{
  registerStaticEntries();

  // Splitting the search string into a char**
  const std::string searchStr = query.getText();

  std::vector<std::string> searchTermsStr = query.getSplitText();

  // Remove tags (i.e. terms that contain ':') because they should be
  // in query.getTags() instead (otherwise, it means they are being typed)
  {
    std::vector<std::string> filteredTerms;
    for( size_t i = 0; i < searchTermsStr.size(); i++ )
      if( searchTermsStr[i].find( ':' ) == std::string::npos )
        filteredTerms.push_back( searchTermsStr[i] );
    searchTermsStr = filteredTerms;
  }

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

  updateSize();
}

const std::string BackdropType = "backdrop";
const std::string VariableSetType = "setVariable";
const std::string VariableGetType = "getVariable";
const char NonPresetPrefix = ':';
const char VariableSeparator = '_';

void DFGPresetSearchWidget::registerStaticEntries()
{
  if( m_staticEntriesAddedToDB || !m_host->isValid() )
    return;

  const char* tags[] = {
    "name:BackDrop",
    "aka:Layout",
    "cat:Tidying",
    "cat:UI"
  };
  m_host->searchDBAddUser(
    (BackdropType + NonPresetPrefix + "BackDrop").data(),
    sizeof( tags ) / sizeof( const char* ),
    tags
  );

  this->registerVariable( "", "" );

  m_staticEntriesAddedToDB = true;
}

std::string GetVariableRegisteredName( const std::string& name, bool isSet )
{
  return ( ( isSet ? VariableSetType : VariableGetType ) + NonPresetPrefix +
    ( isSet ? "Set" : "Get" ) + VariableSeparator + name );
}

void DFGPresetSearchWidget::registerVariable( const std::string& name, const std::string& type )
{
  const std::string nameTag = "name:" + name;
  const std::string typeTag = "porttype:" + type;
  const std::string functions[] = { VariableSetType, VariableGetType };
  for( size_t i = 0; i < sizeof( functions ) / sizeof( std::string ); i++ )
  {
    const std::string& functionType = functions[i];
    std::string registeredName = GetVariableRegisteredName( name, functionType == VariableSetType );
    if( m_registeredVariables.find( registeredName ) != m_registeredVariables.end() )
      continue; // Don't register the same entries several times

    std::vector<const char*> tags;
    tags.push_back( "cat:Variable" );
    tags.push_back( functionType == VariableSetType ? "aka:Set" : "aka:Get" );
    if( name.size() > 0 )
      tags.push_back( nameTag.data() );
    if( type.size() > 0 )
      tags.push_back( typeTag.data() );
    m_host->searchDBAddUser( registeredName.data(), tags.size(), tags.data() );

    m_registeredVariables.insert( registeredName );
  }
}

void DFGPresetSearchWidget::unregisterVariables()
{
  for( std::set<std::string>::const_iterator it = m_registeredVariables.begin();
    it != m_registeredVariables.end(); it++ )
  {
    m_host->searchDBRemoveUser( it->data() );
  }
  m_registeredVariables.clear();
}

void DFGPresetSearchWidget::onResultValidated( const std::string& result )
{
  size_t sep = result.find( NonPresetPrefix );
  if( sep == std::string::npos )
    emit selectedPreset( QString::fromStdString( result ) );
  else
  {
    std::string type = result.substr( 0, sep );
    if( type == BackdropType )
      emit selectedBackdrop();
    else
    if( type == VariableGetType )
      emit selectedGetVariable( result.substr( result.find( VariableSeparator )+1 ) );
    else
    if( type == VariableSetType )
      emit selectedSetVariable( result.substr( result.find( VariableSeparator )+1 ) );

  }
}

void DFGPresetSearchWidget::updateSize()
{
  m_resultsView->setMinimumHeight( std::min(
    m_resultsView->maximumHeight(),
    m_resultsView->sizeHint().height()
  ) );
  m_searchFrame->adjustSize();
  m_searchFrame->adjustSize();
  adjustSize();
  adjustSize();
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
  m_resultPreview->clear();
  updateDetailsPanelVisibility();

  m_status->clear();
  updateSize();
}

void DFGPresetSearchWidget::setPreview( const std::string& preset )
{
  m_resultPreview->setPreset( preset );
  m_detailsPanel->verticalScrollBar()->setValue( 0 );
  updateDetailsPanelVisibility();

  m_status->setText( "<i>" + QString::fromStdString( preset ) + "</i>" );
  m_status->show();
  updateSize();
}

void DFGPresetSearchWidget::updateDetailsPanelVisibility()
{
  m_detailsPanel->setVisible( m_detailsPanelToggled && !m_resultPreview->isEmpty() );
  this->updateSize();
}

void DFGPresetSearchWidget::toggleDetailsPanel( bool toggled )
{
  if( toggled != m_detailsPanelToggled )
  {
    m_detailsPanelToggled = toggled;
    m_toggleDetailsButton->setToggled( toggled );
    updateDetailsPanelVisibility();
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

void DFGPresetSearchWidget::resizeEvent( QResizeEvent *event )
{
  DFGAbstractTabSearchWidget::resizeEvent( event );
  maybeReposition();
}

void DFGPresetSearchWidget::showEvent( QShowEvent * event )
{
  DFGAbstractTabSearchWidget::showEvent( event );
  maybeReposition();
}

void DFGPresetSearchWidget::maybeReposition()
{
  if ( QWidget *parent = parentWidget() )
  {
    QSize parentSize = parent->size();
    // qDebug() << "parentSize" << parentSize;
    QPoint myPos = QPoint(
      // Don't move the widget when shrinking
      std::min( m_posAtShow.x(), this->pos().x() ),
      std::min( m_posAtShow.y(), this->pos().y() )
    );
    // qDebug() << "myPos before" << myPos;
    QSize mySize = size();
    // qDebug() << "mySize" << myPos;
    if ( myPos.x() + mySize.width() > parentSize.width() )
      myPos.setX( std::max( 0, parentSize.width() - mySize.width() ) );
    if ( myPos.y() + mySize.height() > parentSize.height() )
      myPos.setY( std::max( 0, parentSize.height() - mySize.height() ) );
    // qDebug() << "myPos before" << myPos;
    move( myPos );
  }
}

