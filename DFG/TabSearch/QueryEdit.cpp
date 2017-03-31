// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "QueryEdit.h"

#include "ItemView.h"

#include <FTL/JSONValue.h>
#include <qevent.h>
#include <QLineEdit>
#include <QLayout>
#include <iostream>

using namespace FabricUI::DFG::TabSearch;

struct QueryController::Action
{
  virtual void undo() = 0;
  virtual void redo() = 0;
  Query* m_query;
  Action() : m_query( NULL ) {}
  virtual ~Action() {}
};

struct AddTag : QueryController::Action
{
  AddTag( const Query::Tag& tag ) : m_tag( tag ) {}
protected:
  Query::Tag m_tag;
  void undo() FTL_OVERRIDE { m_query->removeTag( m_tag ); }
  void redo() FTL_OVERRIDE { m_query->addTag( m_tag ); }
};

struct RemoveTag : QueryController::Action
{
  RemoveTag( const Query::Tag& tag ) : m_tag( tag ) {}
protected:
  std::string m_tag;
  void undo() FTL_OVERRIDE { m_query->addTag( m_tag ); }
  void redo() FTL_OVERRIDE { m_query->removeTag( m_tag ); }
};

struct SetText : QueryController::Action
{
  SetText(
    const std::string& textAfter,
    const std::string& textBefore
  ) : m_textAfter( textAfter ), m_textBefore( textBefore ) {}
protected:
  std::string m_textAfter, m_textBefore;
  void undo() FTL_OVERRIDE { m_query->setText( m_textBefore ); }
  void redo() FTL_OVERRIDE { m_query->setText( m_textAfter ); }
};

void QueryController::addAndDoAction( QueryController::Action* action )
{
  action->m_query = &m_query;
  action->redo();

  assert( m_currentIndex >= -1 );

  // Cropping actions after
  for( size_t i = size_t( m_currentIndex + 1 ); i<m_stack.size(); i++ )
    delete m_stack[i];
  if( m_stack.size() > size_t( m_currentIndex + 1 ) )
    m_stack.resize( size_t( m_currentIndex + 1 ) );

  m_stack.push_back( action );
  m_currentIndex = int( m_stack.size() ) - 1;
}

QueryController::QueryController( Query& query )
  : m_query( query )
{
  clearStack();
}

QueryController::~QueryController()
{
  clearStack();
}

void QueryController::clearStack()
{
  m_currentIndex = -1;
  for( size_t i = 0; i < m_stack.size(); i++ )
    delete m_stack[i];
  m_stack.resize( 0 );
}

void QueryController::undo()
{
  if( m_currentIndex > -1 )
  {
    if( m_currentIndex < int( m_stack.size() ) )
      m_stack[m_currentIndex]->undo();
    m_currentIndex--;
  }
}

void QueryController::redo()
{
  if( m_currentIndex < int( m_stack.size() ) )
  {
    if( m_currentIndex > -1 )
      m_stack[m_currentIndex]->redo();
    m_currentIndex++;
  }
}

void QueryController::addTag( const Query::Tag& tag )
{
  if( !m_query.hasTag( tag ) )
    addAndDoAction( new AddTag( tag ) );
}

void QueryController::removeTag( const Query::Tag& tag )
{
  assert( m_query.hasTag( tag ) );
  addAndDoAction( new RemoveTag( tag ) );
}

void QueryController::setText( const std::string& text )
{
  if( text != m_query.getText() )
    addAndDoAction( new SetText( text, m_query.getText() ) );
}

void QueryController::clear()
{
  clearStack();
  m_query.clear();
}

class QueryEdit::TagsEdit : public QWidget
{
  void clear()
  {
    for( std::vector<TagWidget*>::iterator it = m_tagViews.begin();
      it != m_tagViews.end(); it++ )
    {
      m_layout->removeWidget( *it );
      ( *it )->deleteLater();
    }
    m_tagViews.clear();
  }

public:
  TagsEdit( QueryController* controller )
    : m_controller( controller )
  {
    m_layout = new QHBoxLayout();
    m_layout->setSpacing( 4 );
    this->setLayout( m_layout );
    this->setObjectName( "TagsEdit" );
    layout()->setAlignment( Qt::AlignLeft );
    clear();
  }

  void setQuery( const Query& query )
  {
    clear();
    const Query::Tags& tags = query.getTags();
    m_layout->setMargin( tags.size() > 0 ? 4 : 0 );
    for( size_t i = 0; i < tags.size(); i++ )
    {
      TagWidget* tagWidget = new TagWidget( tags[i] );
      m_layout->addWidget( tagWidget );
      m_tagViews.push_back( tagWidget );
      connect(
        tagWidget, SIGNAL( activated( const Query::Tag& ) ),
        m_controller, SLOT( removeTag( const Query::Tag& ) )
      );
    }
    this->setVisible( tags.size() > 0 );
  }

  void setHighlightedTag( int index )
  {
    for( size_t i = 0; i < m_tagViews.size(); i++ )
    {
      bool highlighted = index == AllHighlighted || int( i ) == index;
      m_tagViews[i]->setHighlighted( highlighted );
    }
  }
  std::vector<TagWidget*> m_tagViews;
  QHBoxLayout* m_layout;
  QueryController* m_controller;
};

class QueryEdit::TextEdit : public QLineEdit
{
  typedef QLineEdit Parent;

public:
  TextEdit( QueryEdit* parent )
    : m_parent( parent )
  {}

protected:

  // TODO : Refactor the Selection system ?
  void keyPressEvent( QKeyEvent * e ) FTL_OVERRIDE
  {
    if( e->key() == Qt::Key_Up || e->key() == Qt::Key_Down )
      return e->ignore(); // Ignore Up/Down arrow keys (used to navigate through words on OSX)

    if( e->key() == Qt::Key_Space )
    { m_parent->convertTextToTags(); }

    // Undo - Redo
    if( e->matches( QKeySequence::Undo ) )
    {
      m_parent->m_controller->undo();
      return;
    }
    if( e->matches( QKeySequence::Redo ) )
    {
      m_parent->m_controller->redo();
      return;
    }

    // Select all Tags only if the text is already all selected
    if( e->matches( QKeySequence::SelectAll ) )
      if( selectedText() == text() )
        m_parent->m_highlightedTag = AllHighlighted;
      else
        m_parent->m_highlightedTag = NoHighlight;

    if( m_parent->m_highlightedTag == AllHighlighted
      && ( e->key() == Qt::Key_Left || e->key() == Qt::Key_Right ) )
    {
      m_parent->m_highlightedTag = NoHighlight;
    }

    bool navigatingTags = ( cursorPosition() == 0 || e->modifiers().testFlag( Qt::AltModifier ) );

    if( ( e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return ) && isWritingTag() )
      // This will also accept the Event, and it won't be passed to the ResultsView
      m_parent->convertTextToTags();
    else
    if( e->key() == Qt::Key_Backspace )
    {
      int highlitedTag = m_parent->m_highlightedTag; // Selection might change after the text has changed
      if( highlitedTag == NoHighlight || highlitedTag == AllHighlighted )
        Parent::keyPressEvent( e ); // Deleting text
      m_parent->m_highlightedTag = highlitedTag;

      if( m_parent->m_highlightedTag == NoHighlight && cursorPosition() == 0 )
        m_parent->m_highlightedTag = int(m_parent->m_query.getTags().size()) - 1;
      else
      {
        m_parent->removeHighlightedTag();
        if( highlitedTag >= 0 )
          m_parent->m_highlightedTag = highlitedTag - 1;
      }
    }
    else
    // Navigating in the Tags with the arrow keys
    if( navigatingTags && e->key() == Qt::Key_Right )
    {
      if( m_parent->m_highlightedTag == NoHighlight ) // If no Tag is selected
      {
        if( e->modifiers().testFlag( Qt::AltModifier ) && m_parent->m_query.getTags().size() > 0 ) // Alt
          m_parent->m_highlightedTag = 0; // Select the left-most tag
        else
          Parent::keyPressEvent( e ); // Move in the Text
      }
      else
        m_parent->m_highlightedTag++;
    }
    else
    if( navigatingTags && e->key() == Qt::Key_Left )
    {
      if( m_parent->m_highlightedTag == NoHighlight )
        m_parent->m_highlightedTag = int(m_parent->m_query.getTags().size()) - 1;
      else
      // stop at the leftmost tag (since -1 is reserved)
      if( m_parent->m_highlightedTag > 0 )
        m_parent->m_highlightedTag--;
      else
      if( m_parent->m_highlightedTag == 0 )
        m_parent->m_highlightedTag = NoHighlight;
    }
    else
      Parent::keyPressEvent( e );

    m_parent->updateTagHighlight();
  }

  void focusOutEvent( QFocusEvent * e ) FTL_OVERRIDE
  {
    Parent::focusOutEvent( e );
    emit m_parent->lostFocus();
  }

private:
  bool isWritingTag() const
  {
    const std::vector<std::string> split = m_parent->m_query.getSplitText();
    if( split.size() == 0 )
      return false;
    return Query::Tag::IsTag( split[split.size() - 1] );
  }
  QueryEdit* m_parent;
};

QueryEdit::QueryEdit( FabricCore::DFGHost* host )
  : m_host( host )
  , m_tagDBWInitialized( false )
  , m_highlightedTag( NoHighlight )
  , m_controller( new QueryController( m_query ) )
{
  this->setObjectName( "QueryEdit" );
  QVBoxLayout* m_layout = new QVBoxLayout();
  this->setLayout( m_layout );

  m_tagsEdit = new TagsEdit( m_controller );
  m_layout->addWidget( m_tagsEdit );

  m_textEdit = new TextEdit( this );
  m_layout->addWidget( m_textEdit );
  connect(
    m_textEdit, SIGNAL( textChanged( const QString& ) ),
    this, SLOT( onTextChanged( const QString& ) )
  );
  connect(
    &m_query, SIGNAL( changed() ),
    this, SLOT( onQueryChanged() )
  );
  connect(
    m_textEdit, SIGNAL( cursorPositionChanged( int, int ) ),
    this, SLOT( deselectTags() )
  );
  m_layout->setMargin( 0 );
  m_layout->setSpacing( 4 );
  this->setFocusProxy( m_textEdit );

  updateTagDBFromHost();
  onQueryChanged();
}

QueryEdit::~QueryEdit()
{
  delete m_controller;
}
void QueryEdit::onTextChanged( const QString& text )
{
  m_controller->setText( ToStdString( text ) );
}

void QueryEdit::convertTextToTags()
{
  if( !m_tagDBWInitialized )
  {
    updateTagDBFromHost();
    m_tagDBWInitialized = true;
  }

  std::vector< std::pair<size_t, size_t> > indices = m_query.getSplitTextIndices();
  std::string previousText = m_query.getText(), newText = "";
  size_t offset = 0;
  for( size_t i = 0; i < indices.size(); i++ )
  {
    size_t start = indices[i].first, end = indices[i].second;
    const std::string text = previousText.substr( start, end - start );

    bool isTag = false;
    if( Query::Tag::IsTag( text ) )
    {
      isTag = true;
      Query::Tag tag = text;
      if( m_tagDB.find( tag.cat() ) != m_tagDB.end() )
      {
        const TagSet& catTags = m_tagDB[tag.cat()];
        if( catTags.find( tag ) != catTags.end() && !m_query.hasTag( tag ) )
        {
          // the text entered might have the wrong case
          Query::Tag dbName = *( catTags.find( tag ) );
          m_controller->addTag( dbName );
        }
      }
    }
    if( !isTag )
      newText += previousText.substr( offset, end - offset );

    offset = end;
  }
  newText += previousText.substr( offset, previousText.size() - offset );
  m_controller->setText( newText );
}

void QueryEdit::requestTag( const Query::Tag& tag )
{
  m_controller->addTag( tag );
}

void QueryEdit::requestTags( const std::vector<Query::Tag>& tags )
{
  for( size_t i = 0; i < tags.size(); i++ )
    m_controller->addTag( tags[i] );
}

void QueryEdit::clear()
{
  m_controller->clear();
}

void QueryEdit::updateTagHighlight()
{
  // If the TextCursor is not at the beginning
  // or if we overflowed the number of tags :
  // remove the highlight
  if( m_highlightedTag >= int(m_query.getTags().size()) )
    m_highlightedTag = NoHighlight;

  m_tagsEdit->setHighlightedTag( m_highlightedTag );
  if( m_highlightedTag != NoHighlight )
    emit selectingTags();
}

void QueryEdit::deselectTags()
{
  m_highlightedTag = NoHighlight;
  m_tagsEdit->setHighlightedTag( m_highlightedTag );
}

void QueryEdit::removeHighlightedTag()
{
  assert( m_highlightedTag < int(m_query.getTags().size()) );
  if( m_highlightedTag >= 0 ) // If a tag is highlighted, remove it
    m_controller->removeTag( m_query.getTags()[m_highlightedTag] );
  else
  if( m_highlightedTag == AllHighlighted )
  {
    const Query::Tags allTags = m_query.getTags();
    for( size_t i = 0; i < allTags.size(); i++ )
      m_controller->removeTag( allTags[i] );
  }
  m_highlightedTag = NoHighlight;
}

void QueryEdit::selectAll()
{
  m_textEdit->selectAll();
  m_highlightedTag = AllHighlighted;
  updateTagHighlight();
}

void QueryEdit::onQueryChanged()
{
  // Update the QLineEdit, while saving the cursor position
  int textCursor = m_textEdit->cursorPosition();
  m_textEdit->setText( ToQString( m_query.getText() ) );
  m_textEdit->setCursorPosition( textCursor );

  m_tagsEdit->setQuery( m_query );
  updateTagHighlight();
  emit queryChanged( m_query );
}

void QueryEdit::updateTagDBFromHost()
{
  if( !m_host->isValid() )
    return;

  FabricCore::String dbStrR = m_host->dumpPresetSearchDB();
  std::string dbStr( dbStrR.getCStr(), dbStrR.getSize() );
  FTL::JSONValue* db = FTL::JSONValue::Decode( dbStr.c_str() );
  FTL::JSONObject* dbO = db->cast<FTL::JSONObject>();
  for( FTL::JSONObject::const_iterator it = dbO->begin(); it != dbO->end(); it++ )
  {
    Query::Tag tag = std::string( it->key() );
    Query::Tag::Cat cat = tag.cat();
    if( m_tagDB.find( cat ) == m_tagDB.end() )
      m_tagDB.insert( TagDB::value_type( cat, TagSet() ) );
    m_tagDB[cat].insert( tag );
  }
  delete db;

  m_tagDBWInitialized = true;
}
