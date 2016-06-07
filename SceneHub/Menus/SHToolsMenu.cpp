/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHToolsMenu.h"
#include <QtGui/QKeySequence>

using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;
using namespace Menus;

SHToolsMenu::SHToolsMenu(
  SHGLRenderer* shGLRenderer, 
  QWidget *parent)
  : Menus::BaseMenu(shGLRenderer->getClient(), "Tools", parent)
  , m_shGLRenderer(shGLRenderer) {
  QObject::connect(this, SIGNAL(aboutToShow()), this, SLOT(onConstructMenu()));
  m_delimiter = "\t";
}

SHToolsMenu::~SHToolsMenu() {
}

void SHToolsMenu::constructMenu() {
  QList<QStringList> tools = m_shGLRenderer->getRegisteredTools();
  if(tools.size() == 0) return;
  
  for(int i=0; i<tools[0].size(); ++i)
  {
    QAction *toolAction = addAction(tools[0][i] + m_delimiter + tools[1][i] + m_delimiter + tools[2][i]);
    QObject::connect(toolAction, SIGNAL(triggered()), this, SLOT(onActiveTool()));
    toolAction->setCheckable(true);
    if(tools[3][i] == "1")
      toolAction->setChecked(true);
  }
}

void SHToolsMenu::onConstructMenu() {
  clear();
  constructMenu();
}

void SHToolsMenu::onActiveTool() {
  //From the sender action text : name \tab type \tab Key
  //gets the toolKey only
  QAction *action = qobject_cast<QAction *>(QObject::sender());
  QStringList temp = QString(action->text()).split(m_delimiter);
  QString toolKey = temp[2].trimmed();

  //From the toolKey, create a key event
  QKeySequence seq(toolKey);
  QKeyEvent qtKey(QEvent::KeyPress, seq[0], Qt::NoModifier);
     
  //Viewport 0
  m_shGLRenderer->onEvent(0, &qtKey, false);
}
