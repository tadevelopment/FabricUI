//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SCENEHUB_SHVEEDITOROWNER_H__
#define __UI_SCENEHUB_SHVEEDITOROWNER_H__

#include "SHBaseVEEditorOwner.h"
#include <FabricUI/SceneHub/SHStates.h>


namespace FabricUI {
namespace SceneHub {
    
class SHVEEditorOwner : public SHBaseVEEditorOwner {
  Q_OBJECT

  public:
    SHVEEditorOwner(DFG::DFGWidget *dfgWidget, SHStates *states);
    
    virtual ~SHVEEditorOwner();


  public slots:
    virtual void onStructureChanged();
    
    void onInspectChanged();
            
    void onSceneChanged();


  signals:
    void canvasSidePanelInspectRequested();


  protected slots:
    virtual void onSidePanelInspectRequested();


  protected:
    SHStates *m_shStates;

};

} // namespace SceneHub
} // namespace FabricUI

#endif //__UI_SCENEHUB_SHBASEVEEDITOROWNER_H__
