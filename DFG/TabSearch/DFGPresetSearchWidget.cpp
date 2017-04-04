// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DFGPresetSearchWidget.h"
#include "ItemView.h"

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

class DFGPresetSearchWidget::Status : public QWidget
{
  DFGPresetSearchWidget* m_parent;
  std::vector<TabSearch::Label*> m_items;
  TabSearch::Result m_result, m_hoveredResult;
  std::string m_errorMessage;

  inline void addItem( TabSearch::Label* item )
  {
    QObject::connect( item, SIGNAL( requestTag( const Query::Tag& ) ),
      m_parent->m_queryEdit, SLOT( requestTag( const Query::Tag& ) ) );
    this->layout()->addWidget( item );
    m_items.push_back( item );
  }

  void updateDisplay();
  void setErrorStyle( bool error ) { this->setProperty( "error", error ); this->setStyleSheet( this->styleSheet() ); }

public:
  Status( DFGPresetSearchWidget* parent )
    : m_parent( parent )
  {
    this->setObjectName( "Status" );
    QHBoxLayout* lay = new QHBoxLayout();
    lay->setAlignment( Qt::AlignLeft );
    lay->setMargin( 0 );
    lay->setSpacing( 0 );
    this->setLayout( lay );
    this->setMinimumHeight( 18 );
    this->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  }
  void clear()
  {
    for( std::vector<TabSearch::Label*>::const_iterator it = m_items.begin(); it != m_items.end(); it++ )
    {
      this->layout()->removeWidget( *it );
      ( *it )->deleteLater();
    }
    m_items.clear();
  }
  void setDisplayedResult( const TabSearch::Result& );
  void setResult( const TabSearch::Result& result ) { m_result = result; updateDisplay(); }
  void hoveredResultSet( const TabSearch::Result& result ) { m_hoveredResult = result; updateDisplay(); }
  void hoveredResultClear() { hoveredResultSet( TabSearch::Result() ); }
  void setErrorMessage( const std::string& message ) { m_errorMessage = message; updateDisplay(); }
};

void DFGPresetSearchWidget::Status::updateDisplay()
{
  this->setErrorStyle( false );
  if( !m_hoveredResult.empty() )
    setDisplayedResult( m_hoveredResult );
  else
  if( !m_errorMessage.empty() )
  {
    this->clear();
    this->setErrorStyle( true );
    this->addItem( new TabSearch::Label( m_errorMessage ) );
  }
  else
    setDisplayedResult( m_result );
}

DFGPresetSearchWidget::DFGPresetSearchWidget( FabricCore::DFGHost* host )
  : m_clearQueryOnClose( false )
  , m_staticEntriesAddedToDB( false )
  , m_host( host )
  , m_searchFrame( new QFrame() )
  , m_status( new Status( this ) )
  , m_detailsWidget( new TabSearch::DetailsWidget( m_host ) )
  , m_detailsPanel( new QScrollArea() )
  , m_detailsPanelToggled( true )
{

  registerStaticEntries();

  this->setMinimumWidth( 506 );

  this->setObjectName( "DFGPresetSearchWidget" );
  m_searchFrame->setObjectName( "SearchFrame" );
  m_searchFrame->setMinimumWidth( 500 );
  m_searchFrame->setMinimumHeight( 300 );
  m_searchFrame->setMaximumHeight( 300 );
  m_searchFrame->resize( 500, 300 );
  m_detailsPanel->setObjectName( "DetailsPanel" );
  m_detailsPanel->setMaximumHeight( m_searchFrame->maximumHeight() );

  this->setWindowFlags( Qt::Popup );
  QVBoxLayout* vlayout = new QVBoxLayout();

  m_queryEdit = new TabSearch::QueryEdit( m_host );
  m_queryEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  vlayout->addWidget( m_queryEdit );
  connect(
    m_queryEdit, SIGNAL( queryChanged( const TabSearch::Query& ) ),
    this, SLOT( onQueryChanged( const TabSearch::Query& ) )
  );
  connect(
    m_queryEdit, SIGNAL( lostFocus() ),
    this, SLOT( close() )
  );
  connect(
    m_queryEdit, SIGNAL( errorMessage( const std::string& ) ),
    this, SLOT( onErrorMessage( const std::string& ) )
  );

  m_resultsView = new TabSearch::ResultsView();
  m_resultsView->setFocusProxy( this );
  vlayout->addWidget( m_resultsView );
  connect(
    m_resultsView, SIGNAL( presetSelected( const TabSearch::Result& ) ),
    this, SLOT( setPreview( const TabSearch::Result& ) )
  );
  connect(
    m_resultsView, SIGNAL( presetValidated( const TabSearch::Result& ) ),
    this, SLOT( onResultValidated( const TabSearch::Result& ) )
  );
  connect(
    m_resultsView, SIGNAL( presetValidated( const TabSearch::Result& ) ),
    this, SLOT( close() )
  );
  connect(
    m_resultsView, SIGNAL( presetValidated( const TabSearch::Result& ) ),
    this, SIGNAL( giveFocusToParent() )
  );
  connect(
    m_resultsView, SIGNAL( tagRequested( const Query::Tag& ) ),
    m_queryEdit, SLOT( requestTag( const Query::Tag& ) )
  );
  connect(
    m_resultsView, SIGNAL( tagsRequested( const std::vector<Query::Tag>& ) ),
    m_queryEdit, SLOT( requestTags( const std::vector<Query::Tag>& ) )
  );
  connect(
    m_resultsView, SIGNAL( presetDeselected() ),
    this, SLOT( hidePreview() )
  );
  connect(
    m_resultsView, SIGNAL( mouseEnteredPreset( const TabSearch::Result& ) ),
    this, SLOT( onResultMouseEntered( const TabSearch::Result& ) )
  );
  connect(
    m_resultsView, SIGNAL( mouseLeftPreset() ),
    this, SLOT( onResultMouseLeft() )
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
  hlayout->setAlignment( Qt::AlignLeft );
  hlayout->setSizeConstraint( QLayout::SetFixedSize );
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

  m_detailsPanel->setWidget( m_detailsWidget );
  connect(
    m_detailsWidget, SIGNAL( tagRequested( const Query::Tag& ) ),
    m_queryEdit, SLOT( requestTag( const Query::Tag& ) )
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

  vlayout->addWidget( m_status );
  hidePreview();
  updateSize();
}

DFGPresetSearchWidget::~DFGPresetSearchWidget()
{
  this->unregisterVariables();
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
      if( !TabSearch::Query::Tag::IsTag( searchTermsStr[i] ) )
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
const TabSearch::Query::Tag BackdropTag = std::string("name:BackDrop");
const std::string VariableSetType = "setVariable";
const std::string VariableGetType = "getVariable";
const TabSearch::Query::Tag VariableTag = std::string("cat:Variable");
const char VariableSeparator = '_';

void DFGPresetSearchWidget::registerStaticEntries()
{
  if( m_staticEntriesAddedToDB || !m_host->isValid() )
    return;

  const char* tags[] = {
    BackdropTag.data(),
    "aka:Layout",
    "cat:Tidying",
    "cat:UI"
  };

  try
  {
    TabSearch::Result backdropResult( BackdropType, "BackDrop" );
    if( !m_host->searchDBHasUser( backdropResult.data() ) )
      m_host->searchDBAddUser(
        backdropResult.data(),
        sizeof( tags ) / sizeof( const char* ),
        tags
      );
  }
  catch( const FabricCore::Exception& e )
  {
    std::cerr << "DFGPresetSearchWidget::registerStaticEntries : " << e.getDesc_cstr() << std::endl;
    assert( false );
  }

  this->registerVariable( "", "" );

  m_staticEntriesAddedToDB = true;
}

std::string GetVariableRegisteredName( const std::string& name, bool isSet )
{
  return TabSearch::Result( ( isSet ? VariableSetType : VariableGetType ),
    std::string( isSet ? "Set" : "Get" ) + VariableSeparator + name );
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
    if( m_registeredVariables.find( registeredName ) != m_registeredVariables.end() ||
        m_host->searchDBHasUser( registeredName.data() ) )
      continue; // Don't register the same entries several times

    std::vector<const char*> tags;
    tags.push_back( VariableTag.data() );
    tags.push_back( functionType == VariableSetType ? "aka:Set" : "aka:Get" );
    if( name.size() > 0 )
      tags.push_back( nameTag.data() );
    if( type.size() > 0 )
      tags.push_back( typeTag.data() );

    try
    {
      m_host->searchDBAddUser( registeredName.data(), tags.size(), tags.data() );
    }
    catch( const FabricCore::Exception& e )
    {
      std::cerr << "DFGPresetSearchWidget::registerVariable : " << e.getDesc_cstr() << std::endl;
      assert( false );
    }

    m_registeredVariables.insert( registeredName );
  }
}

void DFGPresetSearchWidget::unregisterVariables()
{
  for( std::set<std::string>::const_iterator it = m_registeredVariables.begin();
    it != m_registeredVariables.end(); it++ )
  {
    try
    {
      m_host->searchDBRemoveUser( it->data() );
    }
    catch( const FabricCore::Exception& e )
    {
      std::cerr << "DFGPresetSearchWidget::unregisterVariables : " << e.getDesc_cstr() << std::endl;
      assert( false );
    }
  }
  m_registeredVariables.clear();
  m_staticEntriesAddedToDB = false;
}

void DFGPresetSearchWidget::onResultValidated( const TabSearch::Result& result )
{
  if( result.isPreset() )
    emit selectedPreset( ToQString( result ) );
  else
  {
    const std::string type = result.type();
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
  /*
  m_resultsView->setMinimumHeight( std::min(
    m_resultsView->maximumHeight(),
    m_resultsView->sizeHint().height()
  ) );
  m_searchFrame->adjustSize();
  m_searchFrame->adjustSize();
  adjustSize();
  adjustSize();
  */
}

void DFGPresetSearchWidget::validateSelection()
{
  m_resultsView->validateSelection();
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
  m_detailsWidget->clear();
  updateDetailsPanelVisibility();

  m_status->clear();
  updateSize();
}

void DFGPresetSearchWidget::setPreview( const TabSearch::Result& result )
{
  if( result.isPreset() )
  {
    m_detailsWidget->setPreset( result );
    m_detailsPanel->verticalScrollBar()->setValue( 0 );
    updateDetailsPanelVisibility();
  }
  else
    hidePreview();

  m_status->setResult( result );

  updateSize();
}

void DFGPresetSearchWidget::Status::setDisplayedResult( const TabSearch::Result& result )
{
  this->clear();
  if( result.isPreset() )
  {
    std::set<TabSearch::Query::Tag> validTags = TabSearch::GetTags( result, m_parent->m_host );

    QString s = ToQString( result );
    QStringList parts = s.split( '.', QString::SkipEmptyParts );
    for( int i = 0; i < parts.size(); i++ )
    {
      std::string p = ToStdString( parts[i] );
      bool isName = ( i == parts.size() - 1 );
      TabSearch::Query::Tag tag = isName ?
        TabSearch::Query::Tag( TabSearch::NameCat, p ) :
        TabSearch::Query::Tag( TabSearch::PathCompCat, p )
      ;
      if( validTags.find( tag ) != validTags.end() )
        this->addItem( new TabSearch::Label( p, tag ) );
      else
        this->addItem( new TabSearch::Label( p ) );
      if( !isName )
        this->addItem( new TabSearch::Label( "." ) );
    }
  }
  else
  {
    FTL::StrRef type = result.type();
    if( type == BackdropType )
    {
      this->addItem( new TabSearch::Label( "Add a new " ) );
      this->addItem( new TabSearch::Label( "Backdrop", BackdropTag ) );
    }
    else
    if( type == VariableGetType || type == VariableSetType )
    {
      this->addItem( new TabSearch::Label( "Variable : ", VariableTag ) );
      this->addItem( new TabSearch::Label( result.value() ) );
    }
    else
      assert( false ); // Undefined type
  }
}

void DFGPresetSearchWidget::updateDetailsPanelVisibility()
{
  m_detailsPanel->setVisible( m_detailsPanelToggled && !m_detailsWidget->isEmpty() );
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

void DFGPresetSearchWidget::onResultMouseEntered( const TabSearch::Result& result )
{
  m_status->hoveredResultSet( result );
}

void DFGPresetSearchWidget::onResultMouseLeft()
{
  m_status->hoveredResultClear();
}

void DFGPresetSearchWidget::onErrorMessage( const std::string& message )
{
  m_status->setErrorMessage( message );
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

