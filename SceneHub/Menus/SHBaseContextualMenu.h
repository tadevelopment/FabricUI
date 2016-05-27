/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SCENEHUB_BASECONTEXTUALMENU_H__
#define __FABRICUI_SCENEHUB_BASECONTEXTUALMENU_H__

#include "SHBaseSceneMenu.h"
#include <FabricUI/SceneHub/SHStates.h>
#include <FabricUI/SceneHub/TreeView/SHBaseTreeView.h>

namespace FabricUI {
namespace SceneHub {

class SHBaseContextualMenu : public SHBaseSceneMenu {
  
  /**
    SHBaseContextualMenu specializes SHBaseSceneMenu and defines a base contextual menu.
    This menu can be used from either the treeView or the 3DView (viewports).
    It allowes the edit the SGObjects visibility and to load them recursively.

    Any contextual menu witten in C++ or Python should inheritated from this class.
  */  

  Q_OBJECT
 
  public:
    /// Constructors.
    /// \param shGLScene A pointor to a SHGLScene.
    /// \param shStates A pointor to the SHStates.
    /// \param targetSGObject The SGObject to edit, pass as a RTVal.
    /// \param shBaseTreeView A pointor to the SHTreeView.
    /// \param parent The menu parent, can be null.
    SHBaseContextualMenu(
      SHGLScene* shGLScene,
      SHStates* shStates, 
      FabricCore::RTVal targetSGObject, 
      SHBaseTreeView *shBaseTreeView = 0,
      QWidget *parent = 0);
  
    /// Destructor.
    virtual ~SHBaseContextualMenu();
 
    /// Implementation of BaseMenu.
    virtual void constructMenu();


  protected slots:
    /// Load the object hierarchy.
    void load();
    void loadRecursively();
    void unload();

    /// Resets the object visibility.
    void resetVisibilityRecursively();

    /// Shows the object.
    void showLocal();

    /// Shows the object, propagates to its instances.
    void showPropagated();

    /// Shows the object, parent override.
    void showOverride();

    /// Hides the object.
    void hideLocal();

    /// Hides the object, propagates to its instances.
    void hidePropagated();

    /// Hides the object, parent override.
    void hideOverride();


  protected:
    /// Constructs the menus/actions to recursively
    /// load and expand archives.
    /// Called from constructMenu, can be overriden.
    virtual void constructExpandMenu();

    /// Constructs the menus/actions to edit the SGObjects visibility.
    /// Called from constructMenu, can be overriden.
    virtual void constructVisibilityMenu();

    /// Sets the object visibility.
    /// \param visible If true, show the SGObject.
    /// \param propagationType Propagation type (0:local, 1:propagated, 2:override).
    void setVisibility(bool visible, unsigned int propagationType);
    
    /// Pointor to the SHStates.
    SHStates* m_shStates;
    /// SGObject to edit to edit.
    FabricCore::RTVal m_targetSGObject;
    /// Pointor to the SHTreeView.
    SHBaseTreeView *m_shBaseTreeView;
};

} // SceneHub
} // FabricUI 

#endif // __FABRICUI_SCENEHUB_BASECONTEXTUALMENU_H__
