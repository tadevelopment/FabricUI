//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/DFG/DFGVEEditorOwner.h>
#include <FabricUI/SceneHub/Commands/SHCmdView.h>
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>
#include <FabricUI/SceneHub/TreeView/SHTreeModel.h>

namespace FabricUI {

  namespace SceneHub
  { 
    class SHVEEditorOwner : public DFG::DFGVEEditorOwner
    {
      Q_OBJECT

    public:

      SHVEEditorOwner(DFG::DFGWidget * dfgWidget, SceneHub::SHTreeView * treeView, SHCmdView * cmdView);
      ~SHVEEditorOwner();

      virtual void initConnections();

    signals:

      void canvasSidePanelInspectRequested();

    public slots:

      virtual void onStructureChanged();
      void onSceneItemSelected( FabricUI::SceneHub::SHTreeItem *item );
      void onNewSGObjectSet( FabricCore::RTVal sgObject );
      void onSGObjectPropertyItemInserted( BaseModelItem * item );

    protected slots:

      virtual void onSidePanelInspectRequested();

    private:

      SceneHub::SHTreeView * m_treeView;
      SHCmdView * m_cmdView;
    };
  }
}
