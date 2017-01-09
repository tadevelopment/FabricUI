/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SCENEHUB_TREEVIEWMENU_H__
#define __FABRICUI_SCENEHUB_TREEVIEWMENU_H__

#include <FabricUI/Menus/BaseMenu.h>
#include <FabricUI/SceneHub/TreeView/SHBaseTreeViewsManager.h>

namespace FabricUI {
namespace SceneHub {

class SHTreeViewMenu : public Menus::BaseMenu {
  
  /**
    SHAssetsMenu specializes BaseMenu to edit the SceneHub.SHTreeView items properties.
    It's used to show/hides the properties/generators under the SGObjects.
  */

  Q_OBJECT
 
  public:
    /// Constructor.
    /// \param SHBaseTreeViewsManager Pointor to a SHBaseTreeViewsManager.
    /// \param parent Parent widget, can be null.
    SHTreeViewMenu(
      SHBaseTreeViewsManager* shTreeViewsManager, 
      QWidget *parent = 0);
  
    /// Destructor.
    virtual ~SHTreeViewMenu();

    /// Implementation of BaseMenu
    virtual void constructMenu();


  protected slots:
    /// Shows/hides the treeView items properties/generators
    void updateTreeViewVisibility();


  protected:
    /// Reference to the TreeViewManager.
    SHBaseTreeViewsManager *m_shTreeViewsManager;
    /// \internal
    QAction *m_showProperty;
    /// \internal
    QAction *m_showPropertyGenerator;
};

} // SceneHub
} // FabricUI 

#endif // __FABRICUI_SCENEHUB_TREEVIEWMENU_H__
