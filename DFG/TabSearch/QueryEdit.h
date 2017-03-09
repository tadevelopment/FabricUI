// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_QueryEdit__
#define __UI_DFG_TabSearch_QueryEdit__

#include <QWidget>
#include <FTL/Config.h>
#include <FTL/StrRef.h>
#include <FabricCore.h>
#include <assert.h>
#include <map>
#include <set>

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      // Model
      struct Query : public QObject
      {
        Q_OBJECT

      public:

        class Tag : public std::string
        {
          size_t m_sep;
        public:
          typedef std::string Cat;
          Tag( const std::string& s ) : std::string( s ), m_sep( s.find( ':' ) ) { assert( m_sep != -1 ); }
          inline FTL::StrRef cat() { return FTL::StrRef( data(), m_sep ); }
          inline FTL::StrRef name() { return FTL::StrRef( data() + m_sep + 1, size() - m_sep + 1 ); }
        };

        typedef std::vector<Tag> Tags;
        inline const std::string& getText() const { return m_text; }
        inline const Tags& getTags() const { return m_orderedTags; }
        inline bool hasTag( const std::string& tag ) const { return m_tagMap.find( tag ) != m_tagMap.end(); }
        std::vector< std::pair<size_t,size_t> > getSplitTextIndices() const;
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

      // Controller : used for Undo/Redo
      class QueryController : public QObject
      {
        Q_OBJECT

      public slots:
        void addTag( const std::string& tag );
        void removeTag( const std::string& tag );
        void setText( const std::string& text );
        void clear();
        void undo();
        void redo();

      public:

        QueryController( Query& query );
        ~QueryController();
        struct Action;

      private:
        void addAndDoAction( Action* action );
        void clearStack();
        std::vector<Action*> m_stack;
        int m_currentIndex;
        Query& m_query;
      };

      // View
      class QueryEdit : public QWidget
      {
        Q_OBJECT

        typedef QWidget Parent;

      public:
        QueryEdit( FabricCore::DFGHost* host );
        ~QueryEdit();

      public slots:
        void requestTag( const std::string& tag );
        void requestTags( const std::vector<std::string>& tags );
        void deselectTags();
        void selectAll();
        void clear();
        void removeHighlightedTag();

      signals:
        void queryChanged( const TabSearch::Query& query );
        // Emitted when the widget is moving through its TagsEdit
        void selectingTags();

      private slots:
        void onTextChanged( const QString& text );
        void onQueryChanged();
        void updateTagsEdit();
        void updateTagHighlight();

      private:

        Query m_query;
        class TagsEdit;
        TagsEdit* m_tagsEdit;
        class TextEdit;
        TextEdit* m_textEdit;

        FabricCore::DFGHost* m_host;
        void updateTagDBFromHost();
        typedef std::map< Query::Tag::Cat, std::set<Query::Tag> > TagDB;
        TagDB m_tagDB;
        void convertTextToTags();

        static const int NoHighlight = -1;
        static const int AllHighlighted = -2;
        int m_highlightedTag;
        QueryController* m_controller;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_QueryEdit__
