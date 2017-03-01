// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "QueryEdit.h"

#include "ItemView.h"

#include <qevent.h>
#include <QLineEdit>
#include <QLayout>
#include <iostream>
#include <assert.h>

using namespace FabricUI::DFG::TabSearch;

class QueryEdit::TagsEdit : public QWidget
{
public:
  TagsEdit( const Query& query )
  {
    QHBoxLayout* m_layout = new QHBoxLayout();
    m_layout->setMargin( 0 );
    this->setLayout( m_layout );

    const Query::Tags& tags = query.getTags();
    for( size_t i = 0; i < tags.size(); i++ )
    {
      TagView* tagView = new TagView( tags[i] );
      m_tagViews.push_back( tagView );
      m_layout->addWidget( tagView );
      connect(
        tagView, SIGNAL( activated( const std::string& ) ),
        &query, SLOT( removeTag( const std::string& ) )
      );
    }
  }

  void setHighlightedTag( int index )
  {
    for( size_t i = 0; i < m_tagViews.size(); i++ )
      m_tagViews[i]->setHighlighted( int(i) == index );
  }

private:
  std::vector<TagView*> m_tagViews;
};

class QueryEdit::TextEdit : public QLineEdit
{
  typedef QLineEdit Parent;

public:
  TextEdit( QueryEdit* parent )
    : m_parent( parent )
  {}

protected:
  void keyPressEvent( QKeyEvent * e ) FTL_OVERRIDE
  {
    if( e->key() == Qt::Key_Backspace )
    {
      if( m_parent->m_highlightedTag == -1 && cursorPosition() == 0 )
        m_parent->m_highlightedTag = m_parent->m_query.getTags().size() - 1;
      else
        m_parent->removeHighlightedTag();
    }

    // Navigating in the Tags with the arrow keys
    if( cursorPosition() == 0 && e->key() == Qt::Key_Right )
    {
      if( m_parent->m_highlightedTag == -1 )
        Parent::keyPressEvent( e ); // If no selected Tag, move in the Text
      else
        m_parent->m_highlightedTag++;
    }
    else
    if( cursorPosition() == 0 && e->key() == Qt::Key_Left )
    {
      if( m_parent->m_highlightedTag == -1 )
        m_parent->m_highlightedTag = m_parent->m_query.getTags().size() - 1;
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

QueryEdit::QueryEdit()
  : m_highlightedTag( -1 )
{
  QFont font; font.setPointSize( 16 );
  this->setFont( font );

  QHBoxLayout* m_layout = new QHBoxLayout();
  this->setLayout( m_layout );

  m_tagsEdit = new TagsEdit( m_query );
  this->layout()->addWidget( m_tagsEdit );

  m_textEdit = new TextEdit( this );
  connect(
    m_textEdit, SIGNAL( textChanged( const QString& ) ),
    this, SLOT( onTextChanged( const QString& ) )
  );
  connect(
    &m_query, SIGNAL( changed() ),
    this, SLOT( onQueryChanged() )
  );
  m_layout->addWidget( m_textEdit );
  m_layout->setMargin( 0 );
  this->setFocusProxy( m_textEdit );

  onQueryChanged();
}

void Query::clear()
{
  m_orderedTags.clear();
  m_tagMap.clear();
  emit changed();
}

void Query::addTag( const std::string& tag )
{
  if( m_tagMap.find( tag ) == m_tagMap.end() )
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

void QueryEdit::onTextChanged( const QString& text )
{
  m_query.setText( text.toStdString() );
}

void QueryEdit::requestTag( const std::string& tag )
{
  m_query.addTag( tag );
}

void QueryEdit::requestTags( const std::vector<std::string>& tags )
{
  for( size_t i = 0; i < tags.size(); i++ )
    m_query.addTag( tags[i] );
}

void QueryEdit::clear()
{
  m_query.clear();
}

void QueryEdit::updateTagHighlight()
{
  // If the TextCursor is not at the beginning
  // or if we overflowed the number of tags :
  // remove the highlight
  if( m_textEdit->cursorPosition() > 0 || m_highlightedTag >= int(m_query.getTags().size()) )
    m_highlightedTag = -1;

  m_tagsEdit->setHighlightedTag( m_highlightedTag );
  if( m_highlightedTag != -1 )
    emit selectingTags();
}

void QueryEdit::deselectTags()
{
  m_highlightedTag = -1;
  m_tagsEdit->setHighlightedTag( m_highlightedTag );
}

void QueryEdit::removeHighlightedTag()
{
  assert( m_highlightedTag < m_query.getTags().size() );
  if( m_highlightedTag >= 0 ) // If a tag is highlighted, remove it
    m_query.removeTag( m_query.getTags()[m_highlightedTag] );
  m_highlightedTag = -1;
}

void QueryEdit::updateTagsEdit()
{
  // Remove the widgets from the layout
  layout()->removeWidget( m_tagsEdit );
  layout()->removeWidget( m_textEdit );
  // Delete the TagsEdit and create a new one
  m_tagsEdit->deleteLater();
  m_tagsEdit = new TagsEdit( m_query );
  // Put back the widgets (in the right order)
  layout()->addWidget( m_tagsEdit );
  layout()->addWidget( m_textEdit );
  this->layout()->setSpacing( m_query.getTags().size() == 0 ? 0 : -1 );
}

void QueryEdit::onQueryChanged()
{
  updateTagsEdit();
  m_highlightedTag = -1;
  updateTagHighlight();
  emit queryChanged( m_query );
}
