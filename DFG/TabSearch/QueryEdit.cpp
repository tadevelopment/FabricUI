// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "QueryEdit.h"

#include "ItemView.h"

#include <FTL/JSONValue.h>
#include <qevent.h>
#include <QLineEdit>
#include <QLayout>
#include <iostream>
#include <assert.h>

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
  AddTag( const std::string& name ) : m_tagName( name ) {}
protected:
  std::string m_tagName;
  void undo() FTL_OVERRIDE { m_query->removeTag( m_tagName ); }
  void redo() FTL_OVERRIDE { m_query->addTag( m_tagName ); }
};

struct RemoveTag : QueryController::Action
{
  RemoveTag( const std::string& name ) : m_tagName( name ) {}
protected:
  std::string m_tagName;
  void undo() FTL_OVERRIDE { m_query->addTag( m_tagName ); }
  void redo() FTL_OVERRIDE { m_query->removeTag( m_tagName ); }
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

void QueryController::addTag( const std::string& tag )
{
  if( !m_query.hasTag( tag ) )
    addAndDoAction( new AddTag( tag ) );
}

void QueryController::removeTag( const std::string& tag )
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

struct ArrowedTag
{
  TagView* left;
  TagArrow* right;
  ArrowedTag( TagView* left, TagArrow* right )
    : left( left ), right( right )
  {}
};

class QueryEdit::TagsEdit : public QWidget
{
public:
  TagsEdit( const Query& query, const QueryController* controller )
  {
    QHBoxLayout* m_layout = new QHBoxLayout();
    m_layout->setSpacing( 0 );
    this->setLayout( m_layout );
    this->setObjectName( "TagsEdit" );
    layout()->setAlignment( Qt::AlignLeft );

    const Query::Tags& tags = query.getTags();
    m_layout->setMargin( tags.size() > 0 ? 4 : 0 );
    for( size_t i = 0; i < tags.size(); i++ )
    {
      TagView* tagView = new TagView( tags[i] );
      TagArrow* arrow = new TagArrow();
      arrow->setHasRight( i < tags.size() - 1 );
      m_layout->addWidget( tagView );
      m_layout->addWidget( arrow );
      m_tagViews.push_back( ArrowedTag( tagView, arrow ) );
      connect(
        tagView, SIGNAL( activated( const std::string& ) ),
        controller, SLOT( removeTag( const std::string& ) )
      );
    }
  }

  void setHighlightedTag( int index )
  {
    for( size_t i = 0; i < m_tagViews.size(); i++ )
    {
      bool highlighted = index == AllHighlighted || int( i ) == index;
      m_tagViews[i].left->setHighlighted( highlighted );
      m_tagViews[i].right->setLeftHighlighted( highlighted );
      if( i > 0 )
        m_tagViews[i-1].right->setRightHighlighted( highlighted );
    }
  }

private:
  std::vector<ArrowedTag> m_tagViews;
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
    if( e->matches( QKeySequence::SelectAll ) && selectedText() == text() )
      m_parent->m_highlightedTag = AllHighlighted;

    if( m_parent->m_highlightedTag == AllHighlighted
      && ( e->key() == Qt::Key_Left || e->key() == Qt::Key_Right ) )
    {
      m_parent->m_highlightedTag = NoHighlight;
    }

    if( e->key() == Qt::Key_Backspace )
    {
      int highlitedTag = m_parent->m_highlightedTag; // Selection might change after the text has changed
      Parent::keyPressEvent( e );
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
    if( cursorPosition() == 0 && e->key() == Qt::Key_Right )
    {
      if( m_parent->m_highlightedTag == NoHighlight )
        Parent::keyPressEvent( e ); // If no selected Tag, move in the Text
      else
        m_parent->m_highlightedTag++;
    }
    else
    if( cursorPosition() == 0 && e->key() == Qt::Key_Left )
    {
      if( m_parent->m_highlightedTag == NoHighlight )
        m_parent->m_highlightedTag = int(m_parent->m_query.getTags().size()) - 1;
      else
      // stop at the leftmost tag (since -1 is reserved)
      if( m_parent->m_highlightedTag > 0 )
        m_parent->m_highlightedTag--;
    }
    else
      Parent::keyPressEvent( e );

    m_parent->updateTagHighlight();
  }

private:
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

  m_tagsEdit = new TagsEdit( m_query, m_controller );

  m_textEdit = new TextEdit( this );
  connect(
    m_textEdit, SIGNAL( textChanged( const QString& ) ),
    this, SLOT( onTextChanged( const QString& ) )
  );
  connect(
    &m_query, SIGNAL( changed() ),
    this, SLOT( onQueryChanged() )
  );
  m_layout->setMargin( 0 );
  this->setFocusProxy( m_textEdit );

  updateTagDBFromHost();
  onQueryChanged();
}

QueryEdit::~QueryEdit()
{
  delete m_controller;
}

void Query::clear()
{
  m_orderedTags.clear();
  m_tagMap.clear();
  m_text.clear();
  emit changed();
}

void Query::addTag( const std::string& tag )
{
  if( !hasTag( tag ) )
  {
    m_tagMap.insert( TagMap::value_type( tag, m_orderedTags.size() ) );
    m_orderedTags.push_back( tag );
    emit changed();
  }
}

void Query::removeTag( const std::string& tag )
{
  TagMap::const_iterator item = m_tagMap.find( tag );
  if( item != m_tagMap.end() )
  {
    Tags newTags;
    TagMap newMap;
    size_t indexToRemove = item->second;
    for( size_t i=0; i<m_orderedTags.size(); i++ )
      if( i != indexToRemove )
      {
        newMap.insert( TagMap::value_type( m_orderedTags[i], newTags.size() ) );
        newTags.push_back( m_orderedTags[i] );
      }
    m_orderedTags = newTags;
    m_tagMap = newMap;
    emit changed();
  }
  else
    assert( false );
}

std::vector< std::pair<size_t, size_t> > Query::getSplitTextIndices() const
{
  const std::string& searchStr = getText();
  size_t start = 0;
  std::vector< std::pair<size_t, size_t> > dst;
  for( unsigned int end = 0; end < searchStr.size(); end++ )
  {
    const char c = searchStr[end];
    if( c == '.' || c == ' ' ) // delimiters
    {
      if( end - start > 0 )
        dst.push_back( std::pair<size_t,size_t>( start, end ) );
      start = end + 1;
    }
  }
  if( start < searchStr.size() )
    dst.push_back( std::pair<size_t,size_t>( start, searchStr.size() ) );
  return dst;
}

std::vector<std::string> Query::getSplitText() const
{
  const std::string& searchStr = getText();
  std::vector<std::string> searchTermsStr;
  std::vector< std::pair<size_t, size_t> > indices = getSplitTextIndices();
  for( size_t i = 0; i < indices.size(); i++ )
  {
    size_t start = indices[i].first, end = indices[i].second;
    searchTermsStr.push_back( searchStr.substr( start, end - start ) );
  }
  return searchTermsStr;
}

void QueryEdit::onTextChanged( const QString& text )
{
  m_controller->setText( text.toStdString() );
  convertTextToTags();
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
    if( text.find( ':' ) != std::string::npos
      && end != previousText.size() ) // Ignore tags while they are being written
    {
      Query::Tag tag = text;
      if( m_tagDB.find( tag.cat() ) != m_tagDB.end() )
      {
        const std::set<Query::Tag>& catTags = m_tagDB[tag.cat()];
        if( catTags.find( tag ) != catTags.end() && !m_query.hasTag( tag ) )
        {
          isTag = true;
          m_controller->addTag( tag );
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

void QueryEdit::requestTag( const std::string& tag )
{
  m_controller->addTag( tag );
}

void QueryEdit::requestTags( const std::vector<std::string>& tags )
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
  if( ( m_textEdit->cursorPosition() > 0 && m_highlightedTag != AllHighlighted )
      || m_highlightedTag >= int(m_query.getTags().size()) )
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
    const Query::Tags& allTags = m_query.getTags();
    for( size_t i = 0; i < allTags.size(); i++ )
      m_controller->removeTag( allTags[i] );
  }
  m_highlightedTag = NoHighlight;
}

void QueryEdit::updateTagsEdit()
{
  // Remove the widgets from the layout
  layout()->removeWidget( m_tagsEdit );
  layout()->removeWidget( m_textEdit );
  // Delete the TagsEdit and create a new one
  m_tagsEdit->deleteLater();
  m_tagsEdit = new TagsEdit( m_query, m_controller );
  // Put back the widgets (in the right order)
  layout()->addWidget( m_tagsEdit );
  layout()->addWidget( m_textEdit );
  layout()->setSpacing( m_query.getTags().size() > 0 ? 8 : 0 );
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
  m_textEdit->setText( QString::fromStdString( m_query.getText() ) );
  m_textEdit->setCursorPosition( textCursor );

  updateTagsEdit();
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
    Query::Tag tag = std::string( it->first );
    Query::Tag::Cat cat = tag.cat();
    if( m_tagDB.find( cat ) == m_tagDB.end() )
      m_tagDB.insert( TagDB::value_type( cat, std::set<Query::Tag>() ) );
    m_tagDB[cat].insert( tag );
  }
  delete db;

  m_tagDBWInitialized = true;
}
