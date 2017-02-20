// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ResultsView__
#define __UI_DFG_TabSearch_ResultsView__

#include <QListView>
#include <QStringListModel>

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      class ResultsView : public QListView
      {
        Q_OBJECT

      public:
        ResultsView();

      public slots:
        void setResults( std::vector<std::string> results );

      private:
        QStringListModel m_model;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultsView__
