// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.


#include "SHDFGCombinedWidget.h"
#include <FabricUI/SceneHub/ValueEditor/SHVEEditorOwner.h>
#include <FabricUI/SceneHub/TreeView/SHTreeViewsManager.h>

using namespace FabricUI;
using namespace DFG;


SHDFGCombinedWidget::SHDFGCombinedWidget(QWidget * parent) 
  : DFGCombinedWidget(parent) {
}

void SHDFGCombinedWidget::initDFG() {
  DFGCombinedWidget::initDFG(); 

  m_shStates = new SceneHub::SHStates(m_client);

  m_shDFGBinding = new SceneHub::SHDFGBinding(
    m_mainDFGBinding,
    m_dfgWidget->getDFGController(),
    m_shStates);

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
  // tree view changed -> scene changed
  QObject::connect(m_shTreeViewsManager, SIGNAL(sceneHierarchyChanged()), m_shStates, SLOT(onStateChanged()));
  QObject::connect(m_shTreeViewsManager, SIGNAL(sceneChanged()), m_shStates, SLOT(onStateChanged()));
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
}

void SHDFGCombinedWidget::initDocks() { 
  DFGCombinedWidget::initDocks(); 
  m_hSplitter->addWidget(m_shTreeViewsManager);
}

void SHDFGCombinedWidget::onInspectChanged() {
  // shDFGBinding might change the active binding
  m_shDFGBinding->onInspectChanged();
  FabricCore::DFGBinding binding = m_dfgWidget->getDFGController()->getBinding();
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

}
