//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/DFG/DFGVEEditorOwner.h>
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>
#include <FabricUI/SceneHub/TreeView/SHTreeModel.h>

namespace FabricUI {

  namespace SceneHub
  { 
    class SHVEEditorOwner : public DFG::DFGVEEditorOwner
    {
      Q_OBJECT

    public:

      SHVEEditorOwner(DFG::DFGWidget * dfgWidget, SceneHub::SHTreeView * treeView);
      ~SHVEEditorOwner();

      virtual void initConnections();

    public slots:

      void onSceneItemSelected( FabricUI::SceneHub::SHTreeItem *item );
      void onSGObjectPropertyItemInserted( BaseModelItem * item );

    private:

      // void setModelRoot(
      //   FabricUI::DFG::DFGController *dfgController,
      //   FabricUI::ModelItems::BindingModelItem *bindingModelItem
      //   );

      SceneHub::SHTreeView * m_treeView;
    };
  }
}
