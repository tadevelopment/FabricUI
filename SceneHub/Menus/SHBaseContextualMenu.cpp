/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHBaseContextualMenu.h"
#include <QtGui/QFileDialog>
#include <FabricUI/Util/StringUtils.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;
using namespace Menus;

 
SHBaseContextualMenu::SHBaseContextualMenu(
	SHGLScene* shGLScene,
  SHStates* shStates, 
  FabricCore::RTVal targetSGObject, 
  SHBaseTreeView *shBaseTreeView,
  QWidget *parent)
  : SHBaseSceneMenu(shGLScene, "", parent)
  , m_shStates(shStates)
  , m_targetSGObject(targetSGObject)
  , m_shBaseTreeView(shBaseTreeView) {
}

SHBaseContextualMenu::~SHBaseContextualMenu() {}

void SHBaseContextualMenu::constructMenu() {
  
  // If there is a selected object, either from the shTreeView or the 3View
  // acces/edits its properties.
  QString type(m_targetSGObject.callMethod("String ", "type", 0, 0).getStringCString());

  if(m_targetSGObject.isValid() && type == "SGObject")
  {
    if(m_shBaseTreeView)
    {      
      QAction *expandAction = new QAction("Expand recursively", this);
      addAction(expandAction);
      foreach(QModelIndex index, m_shBaseTreeView->getSelectedIndexes())
      {
        SHTreeView_ViewIndexTarget *viewIndexTarget = new SHTreeView_ViewIndexTarget(m_shBaseTreeView, index, this);
        QObject::connect(expandAction, SIGNAL(triggered()), viewIndexTarget, SLOT(expandRecursively()));
      }
    }

    QAction *loadAction = new QAction("Load recursively", this);
    addAction(loadAction);
    QObject::connect(loadAction, SIGNAL(triggered()), this, SLOT(loadRecursively()));

 
    QMenu *visMenu = addMenu("Visibility");

    QAction *showPropagatedAction = new QAction("Show", visMenu);
    showPropagatedAction->setCheckable(true);
    visMenu->addAction(showPropagatedAction);

    QAction *showLocalAction = new QAction("Show (local only)", visMenu);
    showLocalAction->setCheckable(true);
    visMenu->addAction(showLocalAction);

    QAction *hidePropagatedAction = new QAction("Hide", visMenu);
    hidePropagatedAction->setCheckable(true);
    visMenu->addAction(hidePropagatedAction);

    QAction *hideLocalAction = new QAction("Hide (local only)", visMenu);
    hideLocalAction->setCheckable(true);
    visMenu->addAction(hideLocalAction);

    QAction *resetVisibilityAction = new QAction("Reset recursively", visMenu);
    visMenu->addAction(resetVisibilityAction);

    RTVal propagVal = RTVal::ConstructUInt8(m_client, 0);
    bool visible = m_targetSGObject.callMethod("Boolean", "getVisibility", 1, &propagVal).getBoolean();
    bool propagType = propagVal.getUInt8();
  
    QObject::connect(showLocalAction, SIGNAL(triggered()), this, SLOT(showLocal()));
    if(visible && !propagType)
      showLocalAction->setChecked(true);

    QObject::connect(showPropagatedAction, SIGNAL(triggered()), this, SLOT(showPropagated()));
    if(visible && propagType)
      showPropagatedAction->setChecked(true);

    // THERE ARE BUGS WITH OVERRIDES
    // visAction = new QAction("Show (override)", 0)
    // connect(visAction, SIGNAL(triggered()), self, SLOT(showOverride()))
    // visAction.setCheckable(true)
    // if(visible and propagType == 2)
    //   visAction->setChecked(true)
    // visMenu.addAction(visAction)
        
    QObject::connect(hideLocalAction, SIGNAL(triggered()), this, SLOT(hideLocal()));
    if(!visible && !propagType)
      hideLocalAction->setChecked(true);

    QObject::connect(hidePropagatedAction, SIGNAL(triggered()), this, SLOT(hidePropagated()));
    if(!visible && propagType)
      hidePropagatedAction->setChecked(true);
        
    // THERE ARE BUGS WITH OVERRIDES
    // visAction = new QAction("Hide (override)", 0)
    // connect(visAction, SIGNAL(triggered()), self, SLOT(hideOverride()))
    // visAction.setCheckable(true)
    // if(!visible and propagType == 2)
    //   visAction->setChecked(true)
    // visMenu.addAction(visAction)

    QObject::connect(resetVisibilityAction, SIGNAL(triggered()), this, SLOT(resetVisibilityRecursively()));
  }
}

void SHBaseContextualMenu::loadRecursively() {
 	m_targetSGObject.callMethod("", "forceHierarchyExpansion", 0, 0);
 	m_shStates->onStateChanged();
}

void SHBaseContextualMenu::resetVisibilityRecursively() {
  m_targetSGObject.callMethod("", "resetVisibilityRecursively", 0, 0);
  m_shStates->onStateChanged();
}

void SHBaseContextualMenu::showLocal() {
  setVisibility(true, 0);
}

void SHBaseContextualMenu::showPropagated() {
  setVisibility(true, 1);
}

void SHBaseContextualMenu::showOverride() {
  setVisibility(true, 2);
}

void SHBaseContextualMenu::hideLocal() {
  setVisibility(false, 0);
}

void SHBaseContextualMenu::hidePropagated() {
  setVisibility(false, 1);
}

void SHBaseContextualMenu::hideOverride() {
  setVisibility(false, 2);
}

void SHBaseContextualMenu::setVisibility(bool visible, unsigned int propagationType) { 
	RTVal args[2] = {
 		RTVal::ConstructBoolean(getClient(), visible),
  	RTVal::ConstructUInt32(getClient(), propagationType)
	};
  m_targetSGObject.callMethod("", "setVisibility", 2, args);
  m_shStates->onStateChanged();
}
