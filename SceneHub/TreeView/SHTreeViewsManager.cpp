/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHTreeViewsManager.h"
#include "SHTreeView.h"

using namespace FabricCore;
using namespace FabricUI;
using namespace FabricUI::SceneHub;


SHTreeViewsManager::SHTreeViewsManager(
  FabricCore::Client client,
  DFG::DFGWidget *dfgWidget, 
  SHStates *shStates)
  : SHBaseTreeViewsManager(client, dfgWidget, shStates)
{ 
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(m_client, m_shStates, m_shGLScene);
  QLayout *layout = new QVBoxLayout();
  layout->addWidget(m_comboBox);
  layout->addWidget(m_shTreeView);
  this->setLayout(layout);
  
  QObject::connect(m_shTreeView, SIGNAL(selectionCleared()), this, SLOT(onSelectionCleared()));
  QObject::connect(m_shTreeView, SIGNAL(itemSelected(FabricUI::SceneHub::SHTreeItem *)), this, SLOT(onTreeItemSelected(FabricUI::SceneHub::SHTreeItem *)));
  QObject::connect(m_shTreeView, SIGNAL(itemDeselected(FabricUI::SceneHub::SHTreeItem *)), this, SLOT(onTreeItemDeselected(FabricUI::SceneHub::SHTreeItem *)));
  QObject::connect(m_shTreeView, SIGNAL(itemDoubleClicked(FabricUI::SceneHub::SHTreeItem *)), this, SLOT(onTreeItemDoubleClicked(FabricUI::SceneHub::SHTreeItem *)));
}

SHTreeViewsManager::~SHTreeViewsManager() {
}
