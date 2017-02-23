// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_QueryEdit__
#define __UI_DFG_TabSearch_QueryEdit__

#include <QLineEdit>

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      struct Query
      {
        typedef std::vector<std::string> Tags;
        std::string text;
        Tags tags;
      };

      class QueryEdit : public QLineEdit
      {
        Q_OBJECT

      public:
        QueryEdit();

      signals:
        void queryChanged( const TabSearch::Query& query );

      private slots:
        void onTextChanged( const QString& text );

      private:
        Query m_query;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_QueryEdit__
