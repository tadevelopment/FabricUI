// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_DFGPresetSearchWidget__
#define __UI_DFG_TabSearch_DFGPresetSearchWidget__

#include <FabricUI/DFG/DFGTabSearchWidget.h>

#include "QueryEdit.h"
#include "ResultsView.h"
#include "ResultPreview.h"

namespace FabricUI
{
  namespace DFG
  {
    class DFGPresetSearchWidget : public DFGAbstractTabSearchWidget
    {
      Q_OBJECT

    typedef DFGAbstractTabSearchWidget Parent;

    public:
      DFGPresetSearchWidget( FabricCore::DFGHost* host );

      void showForSearch( QPoint globalPos ) FTL_OVERRIDE;
      void keyPressEvent( QKeyEvent *event ) FTL_OVERRIDE;
      void hideEvent( QHideEvent * e ) FTL_OVERRIDE;
      bool focusNextPrevChild( bool next ) FTL_OVERRIDE;

    signals:
      // Emitted when a Preset has been chosen
      void selectedPreset( QString preset );

    private slots:
      void onQueryChanged( const TabSearch::Query& query );
      void validateSelection();
      void hidePreview();
      void setPreview( QString preset );
      void close();

    private:

      // Will the Query persist between 2 invocations ?
      bool m_clearQueryOnClose;

      // Used to query the database
      FabricCore::DFGHost* m_host;

      TabSearch::QueryEdit* m_queryEdit;
      TabSearch::ResultsView* m_resultsView;
      TabSearch::ResultPreview* m_resultPreview;
    };
  };
};

#endif // __UI_DFG_TabSearch_DFGTabSearchWidget__
