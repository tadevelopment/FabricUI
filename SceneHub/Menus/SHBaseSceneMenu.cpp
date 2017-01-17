/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include "SHBaseSceneMenu.h"
#include <FabricUI/SceneHub/SHGLScene.h>

using namespace FabricUI;
using namespace SceneHub;
using namespace Menus;

SHBaseSceneMenu::SHBaseSceneMenu(
  SHGLScene* shGLScene, 
  QString title, 
  QWidget *parent)
  : BaseMenu(shGLScene->getClient(), title, parent) {

  m_shGLScene = shGLScene;
}

SHBaseSceneMenu::~SHBaseSceneMenu() {}

void SHBaseSceneMenu::constructMenu() {}

SHGLScene* SHBaseSceneMenu::getScene() {
  return m_shGLScene;
}

void SHBaseSceneMenu::onActiveSceneChanged(SHGLScene* shGLScene) {
  m_shGLScene = shGLScene;
}
