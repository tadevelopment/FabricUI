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

inline void GetBasicInfo(
  FabricCore::RTVal handler, 
  QString &name, 
  QString &key, 
  bool &isEnabled) 
{
  name = QString(handler.maybeGetMember("name").getStringCString());
  key = QString(QKeySequence(handler.maybeGetMember("enableKey").getUInt32()).toString());
  isEnabled = handler.maybeGetMember("isEnabled").getBoolean();
}

inline QString GetHandlerType(FabricCore::RTVal handler) {
  unsigned int type(handler.maybeGetMember("handlerType").getUInt32());
  return  type == 0 ? "Shared" : 
          type == 1 ? "Mutually Exclusive" : 
          type == 2 ? "Fully Exclusive" : "Independent";
}

inline void CreateHandlerAction(
  QString actionText, 
  bool isEnabled, 
  QMenu *parent, 
  QMenu *menu) 
{
  QAction *action = menu->addAction(actionText);
  QObject::connect(action, SIGNAL(triggered()), parent, SLOT(onActiveTool()));
  action->setCheckable(true);
  if(isEnabled) action->setChecked(true);
}

void SHToolsMenu::constructMenu() {
  FabricCore::RTVal toolsList = m_shGLRenderer->getRegisteredTools();

  if(!toolsList.isValid()) return;

  if(toolsList.getArraySize() == 0) return;
  
  try 
  {
    for(unsigned int i=0; i<toolsList.getArraySize(); ++i)
    {
      FabricCore::RTVal toolTemp = toolsList.getArrayElement(i);
      QString toolType(toolTemp.callMethod("String", "type", 0, 0).getStringCString());
      FabricCore::RTVal tool = FabricCore::RTVal::Construct(m_client, toolType.toUtf8().constData(), 1, &toolTemp);

      bool isEnabled; QString name, key;
      GetBasicInfo(tool, name, key, isEnabled);
      QString typeStr = GetHandlerType(tool);

      FabricCore::RTVal toolModes = tool.maybeGetMember("modes");
      if(toolModes.getArraySize() == 0)
        CreateHandlerAction(
          QString(name + m_delimiter + typeStr + m_delimiter + key), 
          isEnabled,
          this, 
          this);    
      
      else
      {
        QMenu *toolMenu = new Menus::BaseMenu(m_client, name + m_delimiter + typeStr, this);
         
        CreateHandlerAction(
          QString("EableKey" + m_delimiter + key), 
          isEnabled,
          this, 
          toolMenu);

        for(unsigned int j=0; j<toolModes.getArraySize(); ++j)
        {
          FabricCore::RTVal modeTemp = toolModes.getArrayElement(j);
          QString toolType(modeTemp.callMethod("String", "type", 0, 0).getStringCString());
          FabricCore::RTVal mode = FabricCore::RTVal::Construct(m_client, toolType.toUtf8().constData(), 1, &modeTemp);

          bool isModeEnabled; QString modeName, modekey;
          GetBasicInfo(mode, modeName, modekey, isModeEnabled);
          
          CreateHandlerAction(
            QString(modeName + m_delimiter + modekey), 
            isModeEnabled,
            this, 
            toolMenu);
        }

        addMenu(toolMenu);
      }
    }
  }
  catch(Exception e)
  {
    printf("SHToolsMenu::constructMenu: exception: %s\n", e.getDesc_cstr());
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
  QString toolKey = temp[temp.size()-1].trimmed();

  //From the toolKey, create a key event
  QKeySequence seq(toolKey);
  QKeyEvent qtKey(QEvent::KeyPress, seq[0], Qt::NoModifier);
     
  //Viewport 0
  m_shGLRenderer->onEvent(0, &qtKey, false);
}
