/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SCENEHUB_CONTEXTUALMENU_H__
#define __FABRICUI_SCENEHUB_CONTEXTUALMENU_H__

#include "SHBaseContextualMenu.h"
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>

namespace FabricUI {
namespace SceneHub {

class SHContextualMenu : public SHBaseContextualMenu {
  
  /**
    SHContextualMenu specializes SHBaseContextualMenu for 
      - C++ application
      - DCC plugins
  */

  Q_OBJECT
 
  public:
    /// Constructors.
    /// \param shGLScene A pointor to a SHGLScene.
    /// \param shStates A pointor to the SHStates.
    /// \param targetSGObject The SGObject to edit, pass as a RTVal.
    /// \param shBaseTreeView A pointor to the SHTreeView.
    /// \param parent The menu parent, can be null.
    SHContextualMenu(
      SHGLScene* shGLScene,
      SHStates* shStates, 
      FabricCore::RTVal targetSGObject, 
      SHTreeView *shTreeView = 0,
      QWidget *parent = 0);
  
    /// Destructor.
    virtual ~SHContextualMenu();

    /// Implementation of BaseMenu
    virtual void constructMenu();
};

} // SceneHub
} // FabricUI 

#endif // __FABRICUI_SCENEHUB_CONTEXTUALMENU_H__
