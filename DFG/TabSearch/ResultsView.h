// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_TabSearch_ResultsView__
#define __UI_DFG_TabSearch_ResultsView__

#include <QTreeView>

#include <FTL/Config.h>
#include "Data.h"
#include <map>

namespace FabricUI
{
  namespace DFG
  {
    namespace TabSearch
    {
      class PresetView;

      class ResultsView : public QTreeView
      {
        Q_OBJECT

        typedef QTreeView Parent;

      public:
        ResultsView( FabricCore::DFGHost* );
        ~ResultsView();
        void keyPressEvent( QKeyEvent * ) FTL_OVERRIDE;

      static void UnitTest( const std::string& logFolder = "./" );

      public slots:
        // Sets the result of Search, to be displayed
        void setResults( const std::string& searchResult, const Query& query );
        // Tells that we accept the current selection (on Key_Enter, for example)
        void validateSelection();
        // Removes the current selection
        void clearSelection();

      signals:
        // Emitted when the selection is moved to a non-preset
        void presetDeselected();
        // Emitted when a new preset is selected/highlighted (to preview it, for example)
        void presetSelected( const TabSearch::Result& preset );
        // Emitted when a preset has been chosen
        void presetValidated( const TabSearch::Result& preset );
        // Emitted when a Tag has been requested
        void tagRequested( const Query::Tag& tag );
        void tagsRequested( const std::vector<Query::Tag>& tags );
        // Emitted when moving to a valid selection
        void selectingItems();
        void mouseEnteredPreset( const TabSearch::Result& result );
        void mouseLeftPreset();

      protected slots:
      
        void currentChanged( const QModelIndex &, const QModelIndex & ) FTL_OVERRIDE;

      protected:
        void leaveEvent( QEvent * ) FTL_OVERRIDE;

      private slots:

        void updateHighlight( const QModelIndex& );
        void onEntered( const QModelIndex & );
        void onSelectionChanged();

      private:
        void replaceViewItems( const Query&, const QModelIndex& parent = QModelIndex() );
        const std::string& getSelectedPreset();
        class Model;
        Model* m_model;
        FabricCore::DFGHost* m_host;

        // The void* is QModelIndex::internalPointer()
        class PresetViewItem;
        typedef std::map< void*, PresetViewItem* > PresetViewItems;
        PresetViewItems m_presetViewItems;
        class TagContainer;
        typedef std::map< void*, TagContainer* > TagContainerItems;
        TagContainerItems m_tagContainerItems;
        double minPresetScore, maxPresetScore;
      };
    }
  };
};

#endif // __UI_DFG_TabSearch_ResultsView__
