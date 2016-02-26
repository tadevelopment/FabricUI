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


void SHDFGCombinedWidget::initMenu() {
  DFGCombinedWidget::initMenu();

  QMenu *viewMenu = 0;
  QMenu *fileMenu = 0;
  foreach(QMenu *menu, m_menuBar->findChildren<QMenu*>()) 
  {
    if(std::strcmp(menu->title().toUtf8().constData(), "&File") == 0)
      fileMenu = menu;
    if(std::strcmp(menu->title().toUtf8().constData(), "&View") == 0)
      viewMenu = menu;
  }

  if(fileMenu) 
  {
    foreach( QAction* a, fileMenu->actions() ) {
      if( std::strcmp(a->text().toUtf8().constData(), "Quit") == 0 ) {
        QAction *exportToAlembicAction = new QAction( "Export to Alembic", fileMenu );
        QObject::connect(exportToAlembicAction, SIGNAL( triggered() ), this, SLOT( exportToAlembic() ));
        fileMenu->insertAction( a, exportToAlembicAction );
        fileMenu->insertSeparator( a );
        break;
      }
    }
  }
  else
  { 
    QMenu *fileMenu = m_menuBar->addMenu(tr("&File"));  
    QAction* exportToAlembicAction = fileMenu->addAction("Export to Alembic");
    QObject::connect(exportToAlembicAction, SIGNAL( triggered() ), this, SLOT( exportToAlembic() ));   
  }

  if(viewMenu) 
  {
    viewMenu->addSeparator();
    QAction* showTreeView = viewMenu->addAction("Show TreeView");
    QObject::connect(showTreeView, SIGNAL( triggered() ), this, SLOT( showTreeView() ));   
  }

  FabricCore::RTVal callback;
  if(getRTRHostCallback(callback))
  {
    QMenu *interactionMenu = m_menuBar->addMenu(tr("&Interaction"));  
    FabricCore::RTVal args[2] = {
      FabricCore::RTVal::ConstructVariableArray(m_client, "String"),
      FabricCore::RTVal::ConstructVariableArray(m_client, "Key")
    };
    callback.callMethod("", "getRegisteredTools", 2, args);

    for(uint32_t i=0; i<args[0].getArraySize(); ++i)
    {
      FabricCore::RTVal name = args[0].getArrayElement(i);
      FabricCore::RTVal key = args[1].getArrayElement(i);
      QString str(std::string(std::string(name.getStringCString()) + "\t" + std::string(QKeySequence(key.getUInt32()).toString().toUtf8().constData())).c_str());
      QAction* toolAction = interactionMenu->addAction(str);
      toolAction->setToolTip(str);
      QObject::connect(toolAction, SIGNAL( triggered() ), this, SLOT( activeTool() ));
      QObject::connect(toolAction, SIGNAL( hovered() ), this, SLOT( showToolTip() ));
    }
  }

  QMenu *helpMenu = m_menuBar->addMenu(tr("&Help"));
  QAction *usageAction = helpMenu->addAction("Show Usage");
  QObject::connect(usageAction, SIGNAL( triggered() ), this, SLOT( showUsage() ));
}

void SHDFGCombinedWidget::initTreeView() {
  DFGCombinedWidget::initTreeView();
  m_shTreeView = new FabricUI::SceneHub::SHTreeView(m_client);

  m_LineEdit = new QLineEdit("");
  m_shTreeViewWidget = new QWidget();
  QVBoxLayout *layout = new QVBoxLayout();
  m_shTreeViewWidget->setLayout(layout);
  layout->addWidget(m_LineEdit);
  layout->addWidget(m_shTreeView);
  QObject::connect(m_LineEdit, SIGNAL(editingFinished()), this, SLOT(refreshTreeView()));
}

void SHDFGCombinedWidget::initDocks() { 
  DFGCombinedWidget::initDocks(); 
  m_hSplitter->addWidget(m_shTreeViewWidget);
  m_shTreeViewWidget->hide();
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
  m_shTreeViewWidget->show();
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


// ***** Other *****
void SHDFGCombinedWidget::activeTool() {
  QAction* action = qobject_cast<QAction*>(sender());
  std::string actionName = action->text().toUtf8().constData();
  std::size_t pos = actionName.find("\t");     
  std::string toolName = actionName.substr(0, pos); 
  std::string toolKey = actionName.substr(pos); 
  toolKey.erase(remove(toolKey.begin(), toolKey.end(), '\t'), toolKey.end());

  QKeySequence seq(toolKey.c_str());
  QKeyEvent qtKey( QEvent::KeyPress, seq[0], Qt::NoModifier);
   
  //FABRIC_TRY( "SHDFGCombinedWidget::activeTool",
  //  FabricCore::RTVal viewport = m_glWidget->getViewport();
  //  FabricCore::RTVal klKey = QtToKLEvent(&qtKey, m_client, viewport);
  //  m_shObject.callMethod( "", "onEvent", 1, &klKey );
  //);
}

void SHDFGCombinedWidget::exportToAlembic() {
  /*
  m_timeLine->pause();

  QString filePath = QFileDialog::getSaveFileName(this, "Export scene to Alembic", "", "*.abc");
  if(filePath.length() == 0)
    return;
  if(filePath.toLower().endsWith(".abs.abc"))
    filePath = filePath.left(filePath.length() - 4);
  else if(!filePath.toLower().endsWith(".abc"))
    filePath += ".abc";

  FABRIC_TRY( "SHDFGCombinedWidget::exportToAlembic",
    FabricCore::RTVal filePathVal = FabricCore::RTVal::ConstructString(m_client, filePath.toUtf8().constData());
    m_shObject.callMethod( "", "exportToAlembic", 1, &filePathVal );
  );
  */
}

void SHDFGCombinedWidget::showUsage() {

  QTextEdit *txt = new QTextEdit();
  txt->setText("\
  SceneHub is a work-in-progress app framework for viewing and editing SceneGraph objects.\n\
  This is in an early 'alpha' state and can serve as an example or for testing.\n\
  \n\
  The following operations are supported:\n\
  \n\
  Tree view:\n\
  \n\
  - generated instances will be created as branches are expanded\n\
  - selection will be reflected in the 3D view, enabling the transform manipulator\n\
  - with an item selected, right-clicking will open a contextual menu to expand or load recursively\n\
  \n\
  3D view:\n\
  \n\
  - partially loaded assets are shown as bounding boxes (if at least one of the child isn't loaded)\n\
  \n\
  - hold 'Alt' key to navigate the camera:\n\
  \n\
    - click & drag: orbit\n\
    - middle-click & drag: pan\n\
    - wheel or right-click & drag: zoom\n\
    - arrow keys to orbit and zoom\n\
  \n\
  - click on a geometry to select it. This will activate the transform tool:\n\
  \n\
    - press S, R, T to change transform mode (might need to first click in the 3D view)\n\
    - click & drag the gizmos to change the transform\n\
  \n\
  - With a selected geometry, right-click to have a contextual menu for:\n\
  \n\
    - light: changing the color or intensity\n\
    - geometry: setting the color (shared), local color (instance specific) or texture\n\
    - parent: setting the color (propagated to children)\n\
  \n\
  - click in the background to un-select. Then, right-click to have a contextual menu for:\n\
  \n\
    - adding a geometry from a file (Fbx, Alembic)\n\
    - creating a light\n\
  \n\
  - drag & drop to the 3D view:\n\
  \n\
    - Fbx or Alembic file:\n\
      - adds the asset as a child of the 'root'\n\
      - bounding box'es bottom placed under mouse's cursor (raycast)\n\
      - assets dropped multiple times share their content in the SceneGraph (load once)\n\
      - if holding `Ctrl` key: will load recursively (expand)\n\
  \n\
    - Image file:\n\
      - set as a textured color if dropped on a geometry with UVs\n\
      - images dropped multiple times share their content in the SceneGraph (load once)\n\
      - if holding `Ctrl` key: will set as a local texture (instance specific)\n\
  ");

  txt->resize( 800, 500 );
  txt->setReadOnly( true );
  txt->show();
}
