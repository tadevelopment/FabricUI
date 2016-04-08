/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSETPROPERTYCMD_H__
#define __UI_SCENEHUB_SGSETPROPERTYCMD_H__

#include "SHCmd.h"
using namespace std;
using namespace FabricCore;
 
namespace FabricUI {
namespace SceneHub {


class SGSetPropertyCmd : public SHCmd {

  public:   
    SGSetPropertyCmd() : SHCmd() {
      m_desctiption.cmdName = "setPropertyCmd";
      m_desctiption.cmdType = "SGSetPropertyCmd";
    }

    virtual SHCmdDescription registerCommand() {
      if(QMetaType::type(m_desctiption.cmdType) == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGSetPropertyCmd>(m_desctiption.cmdType);
      return SHCmd::registerCommand();
    }

    virtual QString getFromRTVal(RTVal sgCmd) {
      QString res;
      try 
      {
        RTVal keyVal = RTVal::ConstructString(m_shGLScene->getClient(), "fullPath");
        RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString fullPath = QString(fullPathVal.getStringCString());
        res = QString( m_desctiption.cmdName + "(" + fullPath + ")" );   
      }
      catch(FabricCore::Exception e)
      {
        printf("SGSetPropertyCmd::Get: exception: %s\n", e.getDesc_cstr());
      }
      return res;
    }    

};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGSETPROPERTYCMD_H__
