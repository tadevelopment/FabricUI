// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "Data.h"

using namespace FabricUI::DFG::TabSearch;

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
    for( size_t i = 0; i<m_orderedTags.size(); i++ )
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
        dst.push_back( std::pair<size_t, size_t>( start, end ) );
      start = end + 1;
    }
  }
  if( start < searchStr.size() )
    dst.push_back( std::pair<size_t, size_t>( start, searchStr.size() ) );
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
