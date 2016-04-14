/*
*  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
*/

#ifndef __UI_SCENEHUB_SGADDOBJECTCMD_H__
#define __UI_SCENEHUB_SGADDOBJECTCMD_H__

#include "SHCmd.h"
using namespace FabricCore;

namespace FabricUI {
namespace SceneHub { 
 
class SGAddObjectCmd : public SHCmd {
    
  public:  
    SGAddObjectCmd() : SHCmd() {
      m_description.cmdName = "addObjectCmd";
      m_description.cmdType = "SGAddObjectCmd";
    }

    virtual SHCmdDescription registerCommand() {
      if(QMetaType::type(m_description.cmdType.toUtf8().constData()) == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGAddObjectCmd>(m_description.cmdType.toUtf8().constData());
      return SHCmd::registerCommand();
    }
      
    virtual QString getFromRTVal(RTVal sgCmd) {
      QString cmd;
      try 
      {
        RTVal keyVal = RTVal::ConstructString(m_shGLScene->getClient(), "name");
        RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString name = QString(nameVal.getStringCString());

        keyVal = RTVal::ConstructString(m_shGLScene->getClient(), "isGlobal");
        bool isGlobal = sgCmd.callMethod("Boolean", "getBooleanParam", 1, &keyVal).getBoolean();
        cmd = QString( m_description.cmdName + "(" + name + ", " + QString(isGlobal) + ")" );
      }
      catch(Exception e)
      {
        printf("SGAddObjectCmd::Get: exception: %s\n", e.getDesc_cstr());
      }
      return cmd;
    }
};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGADDOBJECTCMD_H__
