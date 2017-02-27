// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "QueryEdit.h"

#include "ItemView.h"

#include <QLineEdit>
#include <QLayout>
#include <iostream>
#include <assert.h>

using namespace FabricUI::DFG::TabSearch;

class QueryEdit::TagsView : public QWidget
{
public:
  TagsView( const Query& query )
  {
    QHBoxLayout* m_layout = new QHBoxLayout();
    m_layout->setMargin( 0 );
    this->setLayout( m_layout );

    const Query::Tags& tags = query.getTags();
    for( size_t i = 0; i < tags.size(); i++ )
    {
      TagView* tagView = new TagView( tags[i] );
      m_layout->addWidget( tagView );
      connect(
        tagView, SIGNAL( activated( const std::string& ) ),
        &query, SLOT( removeTag( const std::string& ) )
      );
    }
  }
};

QueryEdit::QueryEdit()
{
  QFont font; font.setPointSize( 16 );
  this->setFont( font );

  QHBoxLayout* m_layout = new QHBoxLayout();
  this->setLayout( m_layout );

  m_tagsView = new TagsView( m_query );
  this->layout()->addWidget( m_tagsView );

  QLineEdit* m_textEdit = new QLineEdit();
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

void QueryEdit::clear()
{
  m_query.clear();
}

void QueryEdit::updateTagsView()
{
  TagsView* newTagsView = new TagsView( m_query );
  layout()->replaceWidget( m_tagsView, newTagsView );
  m_tagsView->deleteLater();
  m_tagsView = newTagsView;
  this->layout()->setSpacing( m_query.getTags().size() == 0 ? 0 : -1 );
}

void QueryEdit::onQueryChanged()
{
  updateTagsView();
  emit queryChanged( m_query );
}
