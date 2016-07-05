/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SCENEHUB_TOOLMENU_H__
#define __FABRICUI_SCENEHUB_TOOLMENU_H__

#include <FabricUI/Menus/BaseMenu.h>
#include <FabricUI/SceneHub/SHGLRenderer.h>

namespace FabricUI {
namespace SceneHub {

class SHToolsMenu : public Menus::BaseMenu {
  
  /**
    SHToolsMenu specializes Menus::BaseMenu for 

  */

  Q_OBJECT
 
  public:
    /// Constructor.
    /// \param shGLRenderer A pointor to a SHGLScene.
    /// \param parent The menu parent, can be null.
    SHToolsMenu(SHGLRenderer* shGLRenderer, QWidget *parent = 0);
  
    /// Destructor.
    virtual ~SHToolsMenu();

    /// Implementation of BaseMenu
    virtual void constructMenu();


  public slots:
    void onConstructMenu();

    void onActiveTool();

    void onActiveDispatcher();


  protected:
    QString m_delimiter;

    SHGLRenderer *m_shGLRenderer;
};

} // SceneHub
} // FabricUI 

#endif // __FABRICUI_SCENEHUB_TOOLMENU_H__
