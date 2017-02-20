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
    class DFGTabSearchWidget : public DFGAbstractTabSearchWidget
    {
      Q_OBJECT

      typedef DFGAbstractTabSearchWidget Parent;

    public:
      DFGTabSearchWidget( FabricCore::DFGHost* host );

      void showForSearch( QPoint globalPos ) FTL_OVERRIDE;

      void keyPressEvent( QKeyEvent *event ) FTL_OVERRIDE;

    signals:
      void selectedPreset( QString preset );

    private slots:
      void onQueryChanged( QString query );
      void validateSelection();

    private:
      TabSearch::QueryEdit* m_queryEdit;
      TabSearch::ResultsView* m_resultsView;

      // Used to query the database
      FabricCore::DFGHost* m_host;
    };
  };
};

#endif // __UI_DFG_TabSearch_DFGTabSearchWidget__
