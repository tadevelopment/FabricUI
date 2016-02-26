// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <iostream>

#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QMenuBar>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include "SHDFGCombinedWidget.h"
#include <FabricUI/Style/FabricStyle.h>
#include <FabricUI/DFG/Dialogs/DFGNodePropertiesDialog.h>
#include <FabricUI/DFG/DFGActions.h>

using namespace FabricUI::DFG;
using namespace FabricUI::DFG;

void SHDFGCombinedWidget::initTreeView() {
  DFGCombinedWidget::initTreeView();
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(m_client);
}

void SHDFGCombinedWidget::initDocks() { 
  DFGCombinedWidget::initDocks(); 
  m_LineEdit = new QLineEdit("");
  QWidget *widget = new QWidget();
  QVBoxLayout *layout = new QVBoxLayout();
  widget->setLayout(layout);
  layout->addWidget(m_LineEdit);
  layout->addWidget(m_shTreeView);
  m_hSplitter->addWidget(widget);
  QObject::connect(m_LineEdit, SIGNAL(editingFinished()), this, SLOT(refreshTreeView()));
}

bool SHDFGCombinedWidget::getHost(const char *name, FabricCore::RTVal &shHost) {
  FabricCore::DFGExec exec = m_dfgWidget->getUIController()->getBinding().getExec();
  if(exec.hasVar(name))
  {
    shHost = exec.getVarValue(name);
    return true;
  }
  return false;
}

bool SHDFGCombinedWidget::getRTRHostCallback(FabricCore::RTVal &callback) {
  FabricCore::RTVal isValid = FabricCore::RTVal::ConstructBoolean(m_client, false);   
  callback = FabricCore::RTVal::Construct(m_client, "RTROGLHostCallback", 0, 0);
  callback = callback.callMethod("RTROGLHostCallback", "getCallback", 1, &isValid);
  return isValid.getBoolean();
}

// ***** Tree-View *****
void SHDFGCombinedWidget::refreshTreeView() {

  FABRIC_TRY( "SHDFGCombinedWidget::refreshTreeView",
    FabricCore::RTVal shHost;
    if(getHost(m_LineEdit->text().toUtf8(), shHost))
    {
      FabricCore::RTVal sceneGraphRTVal = shHost.callMethod("SceneGraph", "getScene", 0, 0);
      if(sceneGraphRTVal.isNullObject()) return;

      //NOTE: this doesn't work, returns crap: maybeGetMember("root");
      FabricCore::RTVal sceneRootSGObjectRTVal = sceneGraphRTVal.callMethod("SGObject", "getRoot", 0, 0);
      FabricCore::RTVal materialRootSGObjectRTVal = shHost.callMethod("SGObject", "getMaterialLibraryRoot", 0, 0);
      FabricCore::RTVal imagesRootSGObjectRTVal = shHost.callMethod("SGObject", "getImageLibraryRoot", 0, 0);
      FabricCore::RTVal assetsRootSGObjectRTVal = shHost.callMethod("SGObject", "getAssetLibraryRoot", 0, 0);

      FabricUI::SceneHub::SHTreeModel *treeModel = new FabricUI::SceneHub::SHTreeModel(m_client, sceneGraphRTVal, m_shTreeView);
      QModelIndex sceneRootIndex = treeModel->addRootItem(sceneRootSGObjectRTVal);
      treeModel->addRootItem(materialRootSGObjectRTVal);
      treeModel->addRootItem(imagesRootSGObjectRTVal);
      treeModel->addRootItem(assetsRootSGObjectRTVal);

      m_shTreeView->setModel(treeModel);
      m_shTreeView->setExpanded( sceneRootIndex, true);

      QObject::connect(m_shTreeView, SIGNAL( itemSelected( FabricUI::SceneHub::SHTreeItem * ) ), this, SLOT( treeItemSelected( FabricUI::SceneHub::SHTreeItem * ) ));
      QObject::connect(m_shTreeView, SIGNAL( itemDeselected( FabricUI::SceneHub::SHTreeItem * ) ), this, SLOT( treeItemDeselected( FabricUI::SceneHub::SHTreeItem * ) ));
      QObject::connect(this, SIGNAL( sceneHierarchyChanged() ), treeModel, SLOT( onSceneHierarchyChanged() ));
      QObject::connect(treeModel, SIGNAL( sceneHierarchyChanged() ), this, SLOT( onSceneHierarchyChanged() ));
    
      showTreeView(1);
    }
  );
}

void SHDFGCombinedWidget::showTreeView(unsigned int initalExpandLevel) {
  if(initalExpandLevel == uint32_t(-1)) m_shTreeView->expandAll();
  else if(initalExpandLevel > 0) m_shTreeView->expandToDepth(initalExpandLevel-1);
}

void SHDFGCombinedWidget::showTreeView() {
  showTreeView(1);
}

void SHDFGCombinedWidget::treeItemSelected(FabricUI::SceneHub::SHTreeItem *item) { 
  FABRIC_TRY( "SHDFGCombinedWidget::treeItemSelected",
    FabricCore::RTVal callback;
    if(getRTRHostCallback(callback))
    {
      FabricCore::RTVal sgObj = item->getSGObject();
      callback.callMethod( "", "treeItemSelected", 1, &sgObj );   
      refresh();  
    }
  );
}

void SHDFGCombinedWidget::treeItemDeselected(FabricUI::SceneHub::SHTreeItem *item) {
  FABRIC_TRY( "SHDFGCombinedWidget::treeItemDeselected",
    FabricCore::RTVal callback;
    if(getRTRHostCallback(callback))
    {
      FabricCore::RTVal sgObj = item->getSGObject();
      callback.callMethod( "", "treeItemDeselected", 1, &sgObj );
      refresh();     
    }
  );
}

void SHDFGCombinedWidget::onSceneHierarchyChanged() {
  FabricCore::RTVal shHost;
  if(getHost(m_LineEdit->text().toUtf8(), shHost))
  {
    // Check if it actually changed, to reduce number of notifications
    if(shHost.callMethod("Boolean", "sceneHierarchyChanged", 0, 0).getBoolean())
    {
      m_valueEditor->onOutputsChanged();
      emit sceneHierarchyChanged();
      refresh();
    }
  }
}
