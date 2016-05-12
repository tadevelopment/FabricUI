/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_TREE_VIEW_MANAGER_H__
#define __UI_SCENEHUB_TREE_VIEW_MANAGER_H__

#include "SHBaseTreeViewsManager.h"
 
namespace FabricUI {
namespace SceneHub {

 

class SHTreeViewsManager : public SHBaseTreeViewsManager { 

  /**
    SHTreeViewsManager owns a SHTreeView and allows to display 
    the ojects/properties of a SHGLScene.
    
    The SHGLScenes are defined within the canvas graph as canvas nodes.
    A SHTreeComboBox is used to display all the available scenes in the app and switch among them.
    The treeView is then updated to display the objects of the selected scene.

    The class gives also access to the base signals/slots so it can be specialized if needed.
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
