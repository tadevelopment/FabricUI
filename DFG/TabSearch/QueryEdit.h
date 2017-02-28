// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_QueryEdit__
#define __UI_DFG_TabSearch_QueryEdit__

#include <QWidget>
#include <FTL/Config.h>
#include <map>

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
        typedef std::vector<std::string> Tags;
        inline const std::string& getText() const { return m_text; }
        inline const Tags getTags() const { return m_orderedTags; }

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

      // View
      class QueryEdit : public QWidget
      {
        Q_OBJECT

        typedef QWidget Parent;

      public:
        QueryEdit();

      public slots:
        void requestTag( const std::string& tag );
        void requestTags( const std::vector<std::string>& tags );
        void clear();

      signals:
        void queryChanged( const TabSearch::Query& query );

      private slots:
        void onTextChanged( const QString& text );
        void onQueryChanged();
        void updateTagsEdit();
        void updateTagHighlight();
        void removeHighlightedTag();

      private:

        Query m_query;
        class TagsEdit;
        TagsEdit* m_tagsEdit;
        class TextEdit;
        TextEdit* m_textEdit;

        int m_highlightedTag; // -1 if None
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_QueryEdit__
