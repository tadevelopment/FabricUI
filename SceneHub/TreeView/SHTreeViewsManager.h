/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_TREE_VIEW_MANAGER_H__
#define __UI_SCENEHUB_TREE_VIEW_MANAGER_H__

#include "SHBaseTreeViewsManager.h"
#include <FabricUI/SceneHub/SHStates.h>
#include <FabricUI/SceneHub/SHGLScene.h>
 
namespace FabricUI {
namespace SceneHub {

class SHTreeViewsManager : public SHBaseTreeViewsManager { 

  /**
    SHTreeViewsManager specializes SceneHub.SHBaseTreeViewsManager for C++ applications.
  */

  Q_OBJECT

  public:
    /// Constructor.
    /// \param client A reference to the FabricCore::Client.
    /// \param dfgWidget A reference to the DFG::DFGWidget.
    /// \param shStates A reference to the SceneHub::SHStates.
    SHTreeViewsManager(
      FabricCore::Client client,
      DFG::DFGWidget *dfgWidget, 
      SHStates *shStates);

    virtual ~SHTreeViewsManager();
};

} // namespace SceneHub
} // namespace FabricUI

#endif //__UI_SCENEHUB_TREE_VIEW_MANAGER_H__
