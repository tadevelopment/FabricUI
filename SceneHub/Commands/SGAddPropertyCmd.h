/*
*  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
*/

#ifndef __UI_SCENEHUB_SGADDPROPERTYCMD_H__
#define __UI_SCENEHUB_SGADDPROPERTYCMD_H__

#include "SHCmd.h"
using namespace std;
using namespace FabricCore;

namespace FabricUI {
namespace SceneHub {
 
class SGAddPropertyCmd : public SHCmd {
  
  public:        
    SGAddPropertyCmd() : SHCmd() {
      m_desctiption.cmdName = "addPropertyCmd";
      m_desctiption.cmdType = "SGAddPropertyCmd";
    }

    virtual SHCmdDescription registerCommand() {
      if(QMetaType::type(m_desctiption.cmdType.toUtf8().constData()) == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGAddPropertyCmd>(m_desctiption.cmdType.toUtf8().constData());
      return SHCmd::registerCommand();
    }

    virtual QString getFromRTVal(RTVal sgCmd) {
      QString cmd;
      try 
      {
        RTVal keyVal = RTVal::ConstructString(m_shGLScene->getClient(), "ownerPath");
        RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString ownerPath = QString(nameVal.getStringCString());

        keyVal = RTVal::ConstructString(m_shGLScene->getClient(), "name");
        nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString name = QString(nameVal.getStringCString());
        cmd = QString( m_desctiption.cmdName + "(" + ownerPath + ", " + name + ")" );
      }
      catch(Exception e)
      {
        printf("SGAddPropertyCmd::Get: exception: %s\n", e.getDesc_cstr());
      }
      return cmd;
    }
};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGADDPROPERTYCMD_H__
