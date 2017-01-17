/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SCENEHUB_CONTEXTUALMENU_H__
#define __FABRICUI_SCENEHUB_CONTEXTUALMENU_H__

#include "SHBaseContextualMenu.h"
#include <FabricUI/SceneHub/SHGLRenderer.h>

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
    /// \param shStates A pointor to the SHStates.
    /// \param targetSGObject The SGObject to edit, pass as a RTVal.
    /// \param shBaseTreeView A pointor to the SHBaseTreeView.
    /// \param parent The menu parent, can be null.
    SHContextualMenu(
      SHStates* shStates, 
      FabricCore::RTVal targetSGObject, 
      SHBaseTreeView *shBaseTreeView = 0,
      SHGLRenderer *shGLRenderer = 0,
      QWidget *parent = 0);
  
    /// Destructor.
    virtual ~SHContextualMenu();

    /// Implementation of BaseMenu
    virtual void constructMenu();


  protected:
    SHGLRenderer *m_shGLRenderer;
};

} // SceneHub
} // FabricUI 

#endif // __FABRICUI_SCENEHUB_CONTEXTUALMENU_H__
