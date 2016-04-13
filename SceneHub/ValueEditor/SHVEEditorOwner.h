//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/DFG/DFGVEEditorOwner.h>
#include <FabricUI/SceneHub/DFG/SHDFGBinding.h>
#include <FabricUI/SceneHub/Commands/SHCmdHandler.h>
#include <FabricUI/SceneHub/TreeView/SHBaseTreeView.h>
#include <FabricUI/SceneHub/TreeView/SHTreeModel.h>

namespace FabricUI {
namespace SceneHub {
    
class SGObjectPropertyModelItem;

class SHVEEditorOwner : public DFG::DFGVEEditorOwner {
  Q_OBJECT

  public:
    SHVEEditorOwner(DFG::DFGWidget *dfgWidget, SHBaseTreeView *baseTreeView, SHCmdHandler *cmdViewWidget = 0);
    
    ~SHVEEditorOwner();

    virtual void initConnections();

    void updateSGObject( const FabricCore::RTVal& sgObject );

  signals:
    void canvasSidePanelInspectRequested();


  public slots:
    virtual void onStructureChanged();
    
    void onSceneItemSelected(FabricUI::SceneHub::SHTreeItem *item);
    
    void updateSGObject(SHDFGBinding *shDFGBinding);
    
    void onSGObjectPropertyItemInserted(FabricUI::ValueEditor::BaseModelItem * item);
    
    void onSceneChanged();


  protected slots:
    virtual void onSidePanelInspectRequested();


  protected:

    void updateSGObjectProperty( const FabricCore::RTVal& sgObjectProperty );

    SceneHub::SHBaseTreeView *m_baseTreeView;
    SHCmdHandler *m_cmdViewWidget;
    SGObjectPropertyModelItem *m_objectPropertyItem;//Defined if "root" is a single property
};

} // namespace SceneHub
} // namespace FabricUI
