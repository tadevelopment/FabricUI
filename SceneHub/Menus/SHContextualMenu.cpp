/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHContextualMenu.h"
 
using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;
using namespace Menus;

SHContextualMenu::SHContextualMenu(
  SHGLScene* shGLScene,
  SHStates* shStates, 
  FabricCore::RTVal targetSGObject, 
  SHTreeView *shTreeView,
  QWidget *parent)
  : SHBaseContextualMenu(shGLScene, shStates, targetSGObject, shTreeView, parent) {
}

SHContextualMenu::~SHContextualMenu() {
}

void SHContextualMenu::constructMenu() {
  SHBaseContextualMenu::constructMenu();
}
