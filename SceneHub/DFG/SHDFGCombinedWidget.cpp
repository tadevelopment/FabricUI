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
#include <FabricUI/SceneHub/ValueEditor/SHVEEditorOwner.h>
 
using namespace FabricUI::DFG;


void SHDFGCombinedWidget::initTreeView() {
  DFGCombinedWidget::initTreeView(); 
  m_shTreeViewWidget = new SceneHub::SHTreeViewWidget(
    m_client,
    m_dfgWidget->getUIController(),
    this);
  QObject::connect(m_shTreeViewWidget, SIGNAL(sceneHierarchyChanged()), this, SLOT(onRefreshScene()));
}

void SHDFGCombinedWidget::initDocks() { 
  DFGCombinedWidget::initDocks(); 
  m_hSplitter->addWidget(m_shTreeViewWidget);
}

void SHDFGCombinedWidget::initValueEditor() {
  m_valueEditor = new FabricUI::SceneHub::SHVEEditorOwner( getDfgWidget(), m_shTreeViewWidget->getTreeView(), 0 );
  QObject::connect( m_valueEditor, SIGNAL( log( const char * ) ), this, SLOT( log ( const char * ) ) );
}
