//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_TabSearch_Data__
#define __UI_DFG_TabSearch_Data__

#include <QObject>
#include <FTL/Config.h>
#include <FTL/StrRef.h>
#include <FTL/CStrRef.h>
#include <assert.h>
#include <vector>
#include <map>
#include <set>

namespace FabricCore
{
  class DFGHost;
}

namespace FabricUI {
namespace DFG {
namespace TabSearch {

static const char* NameCat = "name";
static const char* PortTypeCat = "porttype";
static const char* PathCompCat = "pathcomp";
static const char* NameCompCat = "namecomp";

struct Query : public QObject
{
  Q_OBJECT

public:

  class Tag : public std::string
  {
    static const char Sep = ':';
    size_t m_sep;
  public:
    typedef std::string Cat;
    Tag() : std::string(), m_sep( npos ) {}
    Tag( const std::string& cat, const std::string& name )
      : std::string( cat + Sep + name ), m_sep( cat.size() ) { assert( ( *this )[m_sep] = Sep ); }
    Tag( const std::string& s ) : std::string( s ), m_sep( s.find( Sep ) ) { assert( m_sep != npos ); }
    inline FTL::StrRef cat() const
      { assert( m_sep != npos ); return FTL::StrRef( data(), m_sep ); }
    inline FTL::StrRef name() const
      { assert( m_sep != npos ); return FTL::StrRef( data() + m_sep + 1, size() - m_sep - 1 ); }
  };

  typedef std::vector<Tag> Tags;
  inline const std::string& getText() const { return m_text; }
  inline const Tags& getTags() const { return m_orderedTags; }
  inline bool hasTag( const std::string& tag ) const { return m_tagMap.find( tag ) != m_tagMap.end(); }
  std::vector< std::pair<size_t, size_t> > getSplitTextIndices() const;
  std::vector<std::string> getSplitText() const;

  public slots:
  inline void setText( const std::string& text ) { m_text = text; emit changed(); }
  void addTag( const std::string& tag );
  void removeTag( const std::string& tag );
  void clear();

signals:
  void changed();

private:
  std::string m_text;
  Tags m_orderedTags;
  typedef std::map<std::string, size_t> TagMap;
  TagMap m_tagMap;
};

// Can be either a Preset, or another
// result (with a type and a name)
class Result : public std::string
{
  size_t m_sep;
public:
  Result() : m_sep( npos ) {}
  Result( const std::string& s );
  Result( const std::string& type, const std::string& value ); // Non-Preset
  inline bool isPreset() const { return m_sep == npos; }
  inline FTL::StrRef type() const
    { assert( !isPreset() ); return FTL::StrRef( data(), m_sep ); }
  inline FTL::StrRef value() const
    { assert( !isPreset() ); return FTL::StrRef( data() + m_sep + 1, size() - m_sep - 1 ); }
};

std::set<Query::Tag> GetTags( const Result& result, FabricCore::DFGHost* host );

} // namespace TabSearch
} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_TabSearch_Data__
