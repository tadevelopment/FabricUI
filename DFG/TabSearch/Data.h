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

namespace FabricUI {
namespace DFG {
namespace TabSearch {

struct Query : public QObject
{
  Q_OBJECT

public:

  class Tag : public std::string
  {
    size_t m_sep;
  public:
    typedef std::string Cat;
    Tag( const std::string& s ) : std::string( s ), m_sep( s.find( ':' ) ) { assert( m_sep != npos ); }
    inline FTL::StrRef cat() { return FTL::StrRef( data(), m_sep ); }
    inline FTL::StrRef name() { return FTL::StrRef( data() + m_sep + 1, size() - m_sep - 1 ); }
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
  Result( const std::string& s );
  Result( const std::string& type, const std::string& value ); // Non-Preset
  inline bool isPreset() const { return m_sep == npos; }
  inline FTL::StrRef type() const
    { assert( !isPreset() ); return FTL::StrRef( data(), m_sep ); }
  inline FTL::StrRef value() const
    { assert( !isPreset() ); return FTL::StrRef( data() + m_sep + 1, size() - m_sep - 1 ); }
};

} // namespace TabSearch
} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_TabSearch_Data__
