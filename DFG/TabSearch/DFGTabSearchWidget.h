// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_DFGTabSearchWidget__
#define __UI_DFG_TabSearch_DFGTabSearchWidget__

#include <FabricUI/DFG/DFGTabSearchWidget.h>

#include "QueryEdit.h"
#include "ResultsView.h"

namespace FabricUI
{
  namespace DFG
  {
    class DFGTabSearchWidget2 : public DFGAbstractTabSearchWidget
    {
      Q_OBJECT

    public:
      DFGTabSearchWidget2( FabricCore::DFGHost* host );

      void showForSearch( QPoint globalPos ) FTL_OVERRIDE;

    private slots:
      void onQueryChanged( QString query );

    private:
      TabSearch::QueryEdit* m_queryEdit;
      TabSearch::ResultsView* m_resultsView;
      FabricCore::DFGHost* m_host;
    };
  };
};

namespace FabricUI
{
  namespace DFG
  {
    class DFGTabSearchWidget;

    namespace TabSearch
    {
      class QueryEdit;
      class Preview;
      class ResultsView;
    }
  }
}

#endif // __UI_DFG_TabSearch_DFGTabSearchWidget__
