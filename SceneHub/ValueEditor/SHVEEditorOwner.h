//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/DFG/DFGVEEditorOwner.h>
#include <FabricUI/SceneHub/SHDFGCanvas.h>
#include <FabricUI/SceneHub/Commands/SHCmdHandler.h>
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>
#include <FabricUI/SceneHub/TreeView/SHBaseTreeView.h>
#include <FabricUI/SceneHub/TreeView/SHTreeModel.h>

namespace FabricUI {

  namespace SceneHub
  { 
    class SGObjectPropertyModelItem;

    class SHVEEditorOwner : public DFG::DFGVEEditorOwner
    {
      Q_OBJECT

    public:
      SHVEEditorOwner(DFG::DFGWidget *dfgWidget, SHBaseTreeView *baseTreeView, SHCmdHandler *cmdViewWidget = 0);
      
      ~SHVEEditorOwner();

      virtual void initConnections();


    signals:
      void canvasSidePanelInspectRequested();


    public slots:
      virtual void onStructureChanged();
      
      void onSceneItemSelected(FabricUI::SceneHub::SHTreeItem *item);
      
      void updateSGObject(SHDFGCanvas *shDFGCanvas);
      
      void onSGObjectPropertyItemInserted(BaseModelItem * item);
      
      void onSceneChanged();


    protected slots:
      virtual void onSidePanelInspectRequested();


    protected:
      void updateSGObject( const FabricCore::RTVal& sgObject );

      void updateSGObjectProperty( const FabricCore::RTVal& sgObjectProperty );

      SceneHub::SHBaseTreeView *m_baseTreeView;
      SHCmdHandler *m_cmdViewWidget;
      SGObjectPropertyModelItem *m_objectPropertyItem;//Defined if "root" is a single property
    };
  }
}
