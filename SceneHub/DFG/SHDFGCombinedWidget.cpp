// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.


#include "SHDFGCombinedWidget.h"
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>
#include <FabricUI/SceneHub/Menus/SHToolsMenu.h>
#include <FabricUI/SceneHub/Menus/SHTreeViewMenu.h>
#include <FabricUI/SceneHub/Menus/SHContextualMenu.h>
#include <FabricUI/SceneHub/ValueEditor/SHVEEditorOwner.h>
// Julien Keep for debugging
//#include <stdlib.h>
//#include <stdio.h>

using namespace FabricUI;
using namespace DFG;

SHDFGCombinedWidget::SHDFGCombinedWidget(QWidget * parent) 
  : DFGCombinedWidget(parent) {
  // Julien Keep for debugging
  //freopen("myoutpout.txt","a",stdout);
  //freopen("myerror.txt","a",stderr);
}
 
SHDFGCombinedWidget::~SHDFGCombinedWidget() {
}

void SHDFGCombinedWidget::init(      
    FabricCore::Client &client,
    FabricServices::ASTWrapper::KLASTManager *manager,
    FabricCore::DFGHost &host,
    FabricCore::DFGBinding &binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec &exec,
    DFGUICmdHandler *cmdHandler,
    SceneHub::SHGLRenderer *shGLRenderer,
    bool overTakeBindingNotifications,
    DFGConfig config) 
{
  m_shGLRenderer = shGLRenderer;
  DFGCombinedWidget::init(
    client,
    manager,
    host,
    binding,
    execPath,
    exec,
    cmdHandler,
    overTakeBindingNotifications,
    config);
}
 
void SHDFGCombinedWidget::initDFG() {
  DFGCombinedWidget::initDFG(); 
  m_shStates = new SceneHub::SHStates(m_client);

  m_shDFGBinding = new SceneHub::SHDFGBinding(
    m_dfgWidget->getDFGController()->getBinding(),
    m_dfgWidget->getDFGController(),
    m_shStates);

  m_shDFGBinding->setMainBinding(m_dfgWidget->getDFGController()->getBinding());

  QObject::connect(m_shDFGBinding, SIGNAL(sceneChanged()), m_shStates, SLOT(onStateChanged()));
  QObject::connect(m_shStates, SIGNAL(inspectedChanged()), this, SLOT(onInspectChanged()));
}

void SHDFGCombinedWidget::initTreeView() {
  DFGCombinedWidget::initTreeView(); 
  m_shTreeViewsManager = new SceneHub::SHTreeViewsManager(
    m_client,
    m_dfgWidget,
    m_shStates);

  QObject::connect(m_shTreeViewsManager, SIGNAL(activeSceneChanged(FabricUI::SceneHub::SHGLScene *)), this, SLOT(onActiveSceneChanged(FabricUI::SceneHub::SHGLScene *)));
  //scene changed -> tree view changed
  QObject::connect(m_shStates, SIGNAL(sceneHierarchyChanged()), m_shTreeViewsManager, SLOT(onSceneHierarchyChanged()));
  QObject::connect(m_shStates, SIGNAL(selectionChanged()), m_shTreeViewsManager, SLOT(onSelectionChanged()));
  QObject::connect(m_shStates, SIGNAL(selectionChanged()), this, SLOT(onSceneChanged()));
  QObject::connect(m_shStates, SIGNAL(selectionChanged()), m_shTreeViewsManager, SLOT(onSceneChanged()));
  // tree view changed -> scene changed
  QObject::connect(m_shTreeViewsManager, SIGNAL(sceneHierarchyChanged()), m_shStates, SLOT(onStateChanged()));
  QObject::connect(m_shTreeViewsManager, SIGNAL(sceneChanged()), m_shStates, SLOT(onStateChanged()));
  QObject::connect(m_shTreeViewsManager, SIGNAL(sceneChanged()), this, SLOT(onSceneChanged()));

  SceneHub::SHTreeView *treeView = dynamic_cast<SceneHub::SHTreeView *>(m_shTreeViewsManager->getTreeView());
  QObject::connect(
    treeView, 
    SIGNAL(showContextualMenu(QPoint, FabricCore::RTVal, QWidget *, bool)), 
    this, 
    SLOT(onShowContextualMenu(QPoint, FabricCore::RTVal, QWidget *, bool)));
}

void SHDFGCombinedWidget::initValueEditor() {

  FabricUI::SceneHub::SHVEEditorOwner* valueEditor = 
    new FabricUI::SceneHub::SHVEEditorOwner(getDfgWidget(), m_shStates);

  m_valueEditor = valueEditor;
  QObject::connect(valueEditor, SIGNAL(log(const char *)), this, SLOT(log(const char *)));
  QObject::connect(valueEditor, SIGNAL(canvasSidePanelInspectRequested()), this, SLOT(onCanvasSidePanelInspectRequested()));
  QObject::connect(
    valueEditor, 
    SIGNAL(modelItemValueChanged(FabricUI::ValueEditor::BaseModelItem *, QVariant const &)), 
    this, 
    SLOT(onModelValueChanged(FabricUI::ValueEditor::BaseModelItem *, QVariant const &)));

  QObject::connect(m_shStates, SIGNAL(inspectedChanged()), valueEditor, SLOT(onInspectChanged()));
  QObject::connect(m_shStates, SIGNAL(activeSceneChanged()), valueEditor, SLOT(onSceneChanged()));
  QObject::connect(m_shStates, SIGNAL(sceneChanged()), valueEditor, SLOT(onSceneChanged()));
  QObject::connect(m_shStates, SIGNAL(sceneChanged()), this, SLOT(onSceneChanged()));
}

void SHDFGCombinedWidget::initGL() {
  QObject::connect(m_shGLRenderer, SIGNAL(manipsAcceptedEvent(FabricCore::RTVal, bool)), m_shStates, SLOT(onStateChanged()));
  QObject::connect(m_shGLRenderer, SIGNAL(manipsAcceptedEvent(FabricCore::RTVal, bool)), m_shDFGBinding, SLOT(onDriveNodeInputPorts(FabricCore::RTVal)));
  QObject::connect(m_shGLRenderer, SIGNAL(itemDoubleClicked()), m_shStates, SLOT(onInspectSelectedSGObject()));
  QObject::connect(
    m_shGLRenderer, 
    SIGNAL(showContextualMenu(QPoint, FabricCore::RTVal, QWidget *, bool)), 
    this, 
    SLOT(onShowContextualMenu(QPoint, FabricCore::RTVal, QWidget *, bool)));
}

void SHDFGCombinedWidget::initDocks() { 
  DFGCombinedWidget::initDocks(); 
  m_hSplitter->addWidget(m_shTreeViewsManager);
}

void SHDFGCombinedWidget::initMenu() { 
  DFGCombinedWidget::initMenu();

  SceneHub::SHTreeViewMenu *treeViewlMenu = new SceneHub::SHTreeViewMenu(m_shTreeViewsManager);
  treeViewlMenu->constructMenu();
  m_menuBar->addMenu(treeViewlMenu);

  SceneHub::SHToolsMenu *toolMenu = new SceneHub::SHToolsMenu(m_shGLRenderer);
  m_menuBar->addMenu(toolMenu);
}

void SHDFGCombinedWidget::onInspectChanged() {
  //shDFGBinding might change the active binding
  //m_shDFGBinding->onInspectChanged();
  //FabricCore::DFGBinding binding = m_dfgWidget->getDFGController()->getBinding();
}

void SHDFGCombinedWidget::onCanvasSidePanelInspectRequested() {
  if(m_shDFGBinding)
  {
    FabricUI::SceneHub::SHVEEditorOwner* valueEditor = dynamic_cast< FabricUI::SceneHub::SHVEEditorOwner*>(m_valueEditor);
    FabricCore::RTVal parameterObject = m_shDFGBinding->getCanvasOperatorParameterObject();
    if(parameterObject.isValid())
      valueEditor->updateSGObject(parameterObject);
  }
}
 
void SHDFGCombinedWidget::onModelValueChanged(FabricUI::ValueEditor::BaseModelItem * item, QVariant const &newValue) {
  refreshScene();
}

void SHDFGCombinedWidget::onActiveSceneChanged(FabricUI::SceneHub::SHGLScene *scene) {
  refreshScene();
}

void SHDFGCombinedWidget::onSceneChanged() {
  refreshScene();
}

void SHDFGCombinedWidget::onShowContextualMenu(
  QPoint pos, 
  FabricCore::RTVal sgObject, 
  QWidget *parent, 
  bool fromViewport)
{
  SceneHub::SHBaseTreeView *shTreeView = 0;
  SceneHub::SHGLRenderer *shGLRenderer = 0;
  if(!fromViewport) shTreeView = m_shTreeViewsManager->getTreeView();
  else shGLRenderer = m_shGLRenderer;

  SceneHub::SHContextualMenu *menu = new SceneHub::SHContextualMenu(m_shStates, 
    sgObject,
    shTreeView,
    shGLRenderer,
    parent);

  menu->constructMenu();
  menu->exec(pos);
}
