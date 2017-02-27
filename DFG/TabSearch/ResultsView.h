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

        typedef QTreeView Parent;

      public:
        ResultsView();
        ~ResultsView();
        void keyPressEvent( QKeyEvent * ) FTL_OVERRIDE;

      static void UnitTest( const std::string& logFolder = "./" );

      public slots:
        // Sets the result of Search, to be displayed
        void setResults( const std::string& searchResult );
        // Tells that we accept the current selection (on Key_Enter, for example)
        void validateSelection();

      signals:
        // Emitted when the selection is moved to a non-preset
        void presetDeselected();
        // Emitted when a new preset is selected/highlighted (to preview it, for example)
        void presetSelected( QString preset );
        // Emitted when a preset has been chosen
        void presetValidated( QString preset );
        // Emitted when a Tag has been requested
        void tagRequested( const std::string& tag );

      private slots:
      
        void onSelectionChanged();

      private:
        void replaceViewItems( const QModelIndex& parent = QModelIndex() );
        QString getSelectedPreset();
        class Model;
        Model* m_model;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultsView__
