// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ResultsView__
#define __UI_DFG_TabSearch_ResultsView__

#include <QTreeView>

#include <FTL/Config.h>

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      class ResultsView : public QTreeView
      {
        Q_OBJECT

      public:
        ResultsView();
        ~ResultsView();

      public slots:
        // Sets the result of Search, to be displayed
        void setResults( const std::string& searchResult );
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
        inline int numberResults() const { return model()->rowCount(); }
        QString getSelectedPreset();
        class Model;
        Model* m_model;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultsView__
