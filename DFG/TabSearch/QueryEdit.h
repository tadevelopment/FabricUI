// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_QueryEdit__
#define __UI_DFG_TabSearch_QueryEdit__

#include "Data.h"

#include <QWidget>
#include <FTL/Config.h>
#include <FTL/StrRef.h>
#include <FabricCore.h>

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      // Controller : used for Undo/Redo
      class QueryController : public QObject
      {
        Q_OBJECT

      public slots:
        void addTag( const Query::Tag& tag );
        void removeTag( const Query::Tag& tag );
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
        void requestTag( const Query::Tag& tag );
        void requestTags( const std::vector<Query::Tag>& tags );
        void deselectTags();
        void selectAll();
        void clear();
        void removeHighlightedTag();

      signals:
        void queryChanged( const TabSearch::Query& query );
        // Emitted when the widget is moving through its TagsEdit
        void selectingTags();
        void lostFocus();

      private slots:
        void onTextChanged( const QString& text );
        void onQueryChanged();
        void updateTagHighlight();

      private:

        Query m_query;
        class TagsEdit;
        TagsEdit* m_tagsEdit;
        class TextEdit;
        TextEdit* m_textEdit;

        FabricCore::DFGHost* m_host;
        bool m_tagDBWInitialized;
        void updateTagDBFromHost();

        // Case insensitive comparer for tags
        struct CaseInsCmp {
          // TODO : make it a native Query::Tag operator ?
          bool operator() ( const std::string& a, const std::string& b ) const
            { return ( stricmp( a.data(), b.data() ) < 0 ); }
        };

        typedef std::set< Query::Tag, CaseInsCmp > TagSet;
        typedef std::map< Query::Tag::Cat, TagSet, CaseInsCmp > TagDB;
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
