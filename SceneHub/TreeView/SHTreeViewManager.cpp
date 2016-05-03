/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <QtCore/QString>
#include <QtGui/QVBoxLayout>
#include "SHTreeViewManager.h"

using namespace FabricCore;
using namespace FabricUI;
using namespace FabricUI::SceneHub;


SHTreeViewManager::SHTreeViewManager(
  FabricCore::Client client,
  DFG::DFGWidget *dfgWidget, 
  SHStates *shStates)
  : QWidget(0)
  , m_client(client)
  , m_dfgWidget(dfgWidget) 
  , m_shStates(shStates) 
{ 
  m_treeModel = 0;
  m_showOperators = true;
  m_showProperties = true;
  m_bUpdatingSelection = false;

  m_shGLScene = new SHGLScene(m_client);
  m_comboBox = new SHTreeComboBox();
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(m_client, m_shStates, m_shGLScene);
  
  QLayout *layout = new QVBoxLayout();
  layout->addWidget(m_comboBox);
  layout->addWidget(m_shTreeView);
  this->setLayout(layout);
 
  QObject::connect(m_shTreeView, SIGNAL(selectionCleared()), this, SLOT(onSelectionCleared()));
  QObject::connect(m_shTreeView, SIGNAL(itemSelected(FabricUI::SceneHub::SHTreeItem *)), this, SLOT(onTreeItemSelected(FabricUI::SceneHub::SHTreeItem *)));
  QObject::connect(m_shTreeView, SIGNAL(itemDeselected(FabricUI::SceneHub::SHTreeItem *)), this, SLOT(onTreeItemDeselected(FabricUI::SceneHub::SHTreeItem *)));
  QObject::connect(m_shTreeView, SIGNAL(itemDoubleClicked(FabricUI::SceneHub::SHTreeItem *)), this, SLOT(onTreeItemDoubleClicked(FabricUI::SceneHub::SHTreeItem *)));
  QObject::connect(m_comboBox, SIGNAL(updateSceneList()), this, SLOT(onUpdateSceneList()));
  QObject::connect(m_comboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onConstructScene(const QString &)));
  QObject::connect(this, SIGNAL(activeSceneChanged(FabricUI::SceneHub::SHGLScene *)), m_shStates, SLOT(onActiveSceneChanged(FabricUI::SceneHub::SHGLScene *)));
}

SHTreeView* SHTreeViewManager::getTreeView() { 
  return m_shTreeView; 
}

SHGLScene* SHTreeViewManager::getScene() { 
  return m_shGLScene; 
}

void SHTreeViewManager::setShowProperties(bool show) {
  m_showProperties = show;
  if(m_treeModel)
    m_treeModel->setShowProperties(show);
}

void SHTreeViewManager::setShowOperators(bool show) {
  m_showOperators = show;
  if(m_treeModel)
    m_treeModel->setShowOperators(show);
}

void SHTreeViewManager::expandTree(uint32_t level) {
  if(level == uint32_t(-1)) 
    m_shTreeView->expandAll();
  else if(level > 0) 
    m_shTreeView->expandToDepth(level - 1);
}

void SHTreeViewManager::onSceneHierarchyChanged() {
  emit sceneHierarchyChanged();
}

void SHTreeViewManager::onSceneChanged() {
  emit sceneChanged();
}

void SHTreeViewManager::onSelectionCleared() {
  if(!m_bUpdatingSelection)
    m_shStates->clearSelection();
}

void SHTreeViewManager::onConstructScene(const QString &sceneName) {
  if(m_dfgWidget->getDFGController()->getBinding().getExec().hasVar(sceneName.toUtf8().constData()))
  {
    m_shGLScene->setSHGLScene(m_dfgWidget->getDFGController()->getBinding().getExec().getVarValue(sceneName.toUtf8().constData()));
    constructTree();
    emit activeSceneChanged(m_shGLScene);
  }
  else 
  {
    m_comboBox->clear();
    resetTree();
  }
}

void SHTreeViewManager::onUpdateSceneList() {
  m_comboBox->clear();
  QStringList sceneNameList = m_shGLScene->getSceneNamesFromBinding(m_dfgWidget->getDFGController()->getBinding());
      
  if(sceneNameList.size() == 0)
    resetTree();
 
  for(int i=0; i<sceneNameList.size(); ++i)
    m_comboBox->addItem(sceneNameList[i]);
}

void SHTreeViewManager::onTreeItemSelected(FabricUI::SceneHub::SHTreeItem *item) {
  if(!m_bUpdatingSelection)
  {
    m_bUpdatingSelection = true;

    if(item->isReference())
    {
      RTVal val = item->getSGObject();
      if(val.isValid())
        m_shStates->addSGObjectToSelection(val);
    }
    else
    {
      RTVal val = item->getSGObjectProperty();
      if(val.isValid())
      {
        if(item->isGenerator())
          m_shStates->addSGObjectPropertyGeneratorToSelection(val);
        else
          m_shStates->addSGObjectPropertyToSelection(val);
      }
    }

    m_bUpdatingSelection = false;
  }
}

void SHTreeViewManager::onTreeItemDeselected(FabricUI::SceneHub::SHTreeItem *item) {
  if(!m_bUpdatingSelection)
  {
    m_bUpdatingSelection = true;

    if(item->isReference())
    {
      RTVal val = item->getSGObject();
      if(val.isValid())
        m_shStates->removeSGObjectFromSelection(val);
    }
    else
    {
      RTVal val = item->getSGObjectProperty();
      if(val.isValid())
      {
        if(item->isGenerator())
          m_shStates->removeSGObjectPropertyGeneratorFromSelection(val);
        else
          m_shStates->removeSGObjectPropertyFromSelection(val);
      }
    }

    m_bUpdatingSelection = false;
  }
}

void SHTreeViewManager::onTreeItemDoubleClicked(FabricUI::SceneHub::SHTreeItem *item) {
  if(!m_bUpdatingSelection)
  {
    m_bUpdatingSelection = true;

    if(item->isReference())
    {
      RTVal val = item->getSGObject();
      if(val.isValid())
        m_shStates->onInspectedSGObject(val);
    }
    else
    {
      RTVal val = item->getSGObjectProperty();
      if(val.isValid())
      {
        if(item->isGenerator())
          m_shStates->onInspectedSGObjectPropertyGenerator(val);
        else
          m_shStates->onInspectedSGObjectProperty(val);
      }
    }

    m_bUpdatingSelection = false;

  }
}

void SHTreeViewManager::onSelectionChanged() {
  if(!m_bUpdatingSelection)
  {
    m_bUpdatingSelection = true;
    m_shTreeView->setSelectedObjects(m_shStates->getSelectedObjects());
    m_bUpdatingSelection = false;
  }
}

void SHTreeViewManager::resetTree() {
  m_shTreeView->reset();
  if(m_treeModel)
  {
    delete m_treeModel;
    m_treeModel = 0;
  }
}

void SHTreeViewManager::constructTree() {
  resetTree();

  m_treeModel = new FabricUI::SceneHub::SHTreeModel(
    m_shGLScene->getClient(), 
    m_shGLScene->getSG(), 
    m_shTreeView);

  setShowProperties(m_showProperties);
  setShowOperators(m_showOperators);

  QObject::connect(this, SIGNAL(sceneHierarchyChanged()), m_treeModel, SLOT(onSceneHierarchyChanged()));
  QObject::connect(m_treeModel, SIGNAL(sceneHierarchyChanged()), this, SLOT(onSceneHierarchyChanged()));
  QObject::connect(m_treeModel, SIGNAL(sceneChanged()), this, SLOT(onSceneChanged()));

  QModelIndex sceneRootIndex = m_treeModel->addRootItem(m_shGLScene->getSceneRoot());
  m_treeModel->addRootItem(m_shGLScene->getAssetLibraryRoot());
  m_treeModel->addRootItem(m_shGLScene->getMaterialLibraryRoot());
  m_treeModel->addRootItem(m_shGLScene->getImageLibraryRoot());

  m_shTreeView->setModel(m_treeModel);
  m_shTreeView->setExpanded(sceneRootIndex, true);
}
