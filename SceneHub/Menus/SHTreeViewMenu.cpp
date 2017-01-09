/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include "SHTreeViewMenu.h"
 
using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;
using namespace Menus;

SHTreeViewMenu::SHTreeViewMenu(
  SHBaseTreeViewsManager* shTreeViewsManager, 
  QWidget *parent)
  : Menus::BaseMenu(shTreeViewsManager->getClient(), "Tree View", parent)
  , m_shTreeViewsManager(shTreeViewsManager) {
}

SHTreeViewMenu::~SHTreeViewMenu() {
}

void SHTreeViewMenu::constructMenu() {
  m_showProperty = addAction("Show properties");
  m_showProperty->setCheckable(true);
  m_showProperty->setChecked(true);
  m_showPropertyGenerator = addAction("Show property generators");
  m_showPropertyGenerator->setCheckable(true);
  m_showPropertyGenerator->setChecked(true);
  QObject::connect(m_showProperty, SIGNAL(triggered()), this, SLOT(updateTreeViewVisibility()));
  QObject::connect(m_showPropertyGenerator, SIGNAL(triggered()), this, SLOT(updateTreeViewVisibility()));
}

void SHTreeViewMenu::updateTreeViewVisibility() {
  m_shTreeViewsManager->setShowProperties(m_showProperty->isChecked());
  m_shTreeViewsManager->setShowOperators(m_showPropertyGenerator->isChecked());
}
