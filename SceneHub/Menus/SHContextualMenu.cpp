/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHContextualMenu.h"
#include <FabricUI/SceneHub/Menus/SHToolsMenu.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;
using namespace Menus;

SHContextualMenu::SHContextualMenu(
  SHStates* shStates, 
  FabricCore::RTVal targetSGObject, 
  SHBaseTreeView *shBaseTreeView,
  SHGLRenderer *shGLRenderer,
  QWidget *parent)
  : SHBaseContextualMenu(shStates, targetSGObject, shBaseTreeView, parent) {

  m_shGLRenderer = 0;
  if(shGLRenderer) m_shGLRenderer = shGLRenderer;
}

SHContextualMenu::~SHContextualMenu() {
}

void SHContextualMenu::constructMenu() {
  SHBaseContextualMenu::constructMenu();
  if(m_shGLRenderer)
  {
    SHToolsMenu *toolsMenu = new SHToolsMenu(m_shGLRenderer);
    addMenu(toolsMenu);
  }
}
