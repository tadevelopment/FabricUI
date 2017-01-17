/*
*  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
*/

#ifndef __UI_SCENEHUB_SGADDOBJECTCMD_H__
#define __UI_SCENEHUB_SGADDOBJECTCMD_H__

#include "SHCmd.h"
using namespace FabricCore;

namespace FabricUI {
namespace SceneHub { 
 
class SGAddObjectCmd : public SHCmd {
    
  public:  
    SGAddObjectCmd() : SHCmd() {}

    virtual void registerCommand() {
      if(QMetaType::type("SGAddObjectCmd") == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGAddObjectCmd>("SGAddObjectCmd");
    }
      
    virtual void setFromRTVal(Client client, RTVal sgCmd) {
      try 
      {
        SHCmd::setFromRTVal(client, sgCmd);
        RTVal keyVal = RTVal::ConstructString(m_client, "name");
        RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString name = QString(nameVal.getStringCString());

        keyVal = RTVal::ConstructString(m_client, "isGlobal");
        bool isGlobal = sgCmd.callMethod("Boolean", "getBooleanParam", 1, &keyVal).getBoolean();
        m_description += QString( "(" + name + ", " + QString(isGlobal) + ")" );
      }
      catch(Exception e)
      {
        printf("SGAddObjectCmd::setFromRTVal: exception: %s\n", e.getDesc_cstr());
      }
    }
};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGADDOBJECTCMD_H__
