//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/DFG/DFGVEEditorOwner.h>
#include <FabricUI/SceneHub/Commands/SHCmdViewWidget.h>
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>
#include <FabricUI/SceneHub/TreeView/SHTreeViewWidget.h>
#include <FabricUI/SceneHub/TreeView/SHTreeModel.h>

namespace FabricUI {

  namespace SceneHub
  { 
    class SGObjectPropertyModelItem;

    class SHVEEditorOwner : public DFG::DFGVEEditorOwner
    {
      Q_OBJECT

    public:

      SHVEEditorOwner(DFG::DFGWidget * dfgWidget, SceneHub::SHTreeViewWidget * treeViewWidget, SHCmdViewWidget * cmdViewWidget);
      ~SHVEEditorOwner();

      virtual void initConnections();

    signals:

      void canvasSidePanelInspectRequested();

    public slots:

      virtual void onStructureChanged();
      void onSceneItemSelected( FabricUI::SceneHub::SHTreeItem *item );
      void updateSGObject( const FabricCore::RTVal& sgObject );
      void onSGObjectPropertyItemInserted( BaseModelItem * item );
      void onSceneChanged();

    protected slots:

      virtual void onSidePanelInspectRequested();

    private:

      void updateSGObjectProperty( const FabricCore::RTVal& sgObjectProperty );

      SceneHub::SHTreeViewWidget * m_treeViewWidget;
      SHCmdViewWidget * m_cmdViewWidget;
      SGObjectPropertyModelItem * m_objectPropertyItem;//Defined if "root" is a single property
    };
  }
}
