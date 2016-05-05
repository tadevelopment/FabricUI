/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SCENEHUB_BASESCENEMENU_H__
#define __FABRICUI_SCENEHUB_BASESCENEMENU_H__

#include <FabricUI/Menus/BaseMenu.h>
#include <FabricUI/SceneHub/SHGLScene.h>

namespace FabricUI {
namespace SceneHub {

class SHBaseSceneMenu : public Menus::BaseMenu {
  
  /**
    SHBaseSceneMenu specializes BaseMenu.
    It owns a pointor to a SceneHub.SHGLScene scene to edit
    and defines the onActiveSceneChanged method to update it. 
  */

  Q_OBJECT
 
  public:
    /// Constructors.
    /// \param shGLScene A pointor to a SHGLScene.
    /// \param title The menu title, can be None.
    /// \param parent The menu widget parent, can be null.
    SHBaseSceneMenu(
      SHGLScene* shGLScene, 
      QString title, 
      QWidget *parent = 0);
    
    /// Destructor.
    virtual ~SHBaseSceneMenu();

    /// Gets a reference to the SHGLScene.
    SHGLScene* getScene();

    /// Implementation of BaseMenu.
    virtual void constructMenu();
    

  public slots:
    /// Updates the SHGLScene to edit.
    /// \param shGLScene A pointor to a SHGLScene.
    void onActiveSceneChanged(FabricUI::SceneHub::SHGLScene *shGLScene);


  protected:
    /// Pointor to the SHGLScene to edit.
    SHGLScene *m_shGLScene;
};

} // SceneHub
} // FabricUI 

#endif // __FABRICUI_SCENEHUB_BASESCENEMENU_H__
