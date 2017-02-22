// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ResultsView__
#define __UI_DFG_TabSearch_ResultsView__

#include <QListView>
#include <QStringListModel>

#include <FTL/Config.h>

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
        // Sets the result of Search, to be displayed
        void setResults( const std::vector<std::string>& results );
        // Moves the current selection up/down
        void moveSelection( int increment = +1 );
        // Tells that we accept the current selection (on Key_Enter, for example)
        void validateSelection();

      signals:
        // Emitted when a new preset is selected/highlighted (to preview it, for example)
        void presetSelected( QString preset );
        // Emitted when a preset has been chosen
        void presetValidated( QString preset );

      private slots:
        void selectionChanged();

      private:
        void setSelection( unsigned int index );
        inline int numberResults() const { return m_model.rowCount(); }
        QString getSelectedPreset();
        QStringListModel m_model;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultsView__
