//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include "SHBaseVEEditorOwner.h"
#include <FabricUI/SceneHub/TreeView/SHBaseTreeView.h>

namespace FabricUI {
namespace SceneHub {
    

class SHVEEditorOwner : public SHBaseVEEditorOwner {
  Q_OBJECT

  public:
    SHVEEditorOwner(DFG::DFGWidget *dfgWidget, SHBaseTreeView *baseTreeView);
    
    virtual ~SHVEEditorOwner();

    virtual void initConnections();


  signals:
    void canvasSidePanelInspectRequested();


  public slots:
    virtual void onStructureChanged();
    
    void onSceneItemSelected(FabricUI::SceneHub::SHTreeItem *item);
            
    void onUpdateSGObject(SHDFGBinding *shDFGBinding);

    void onSceneChanged();


  protected slots:
    virtual void onSidePanelInspectRequested();


  protected:
    SceneHub::SHBaseTreeView *m_baseTreeView;

};

} // namespace SceneHub
} // namespace FabricUI
