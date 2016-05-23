/*
*  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
*/

#ifndef __UI_SCENEHUB_SGADDPROPERTYCMD_H__
#define __UI_SCENEHUB_SGADDPROPERTYCMD_H__

#include "SHCmd.h"
using namespace FabricCore;

namespace FabricUI {
namespace SceneHub {
 
class SGAddPropertyCmd : public SHCmd {
  
  public:        
    SGAddPropertyCmd() : SHCmd() {}

    virtual void registerCommand() {
      if(QMetaType::type("SGAddPropertyCmd") == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGAddPropertyCmd>("SGAddPropertyCmd");
    }

    virtual void setFromRTVal(Client client, RTVal sgCmd) {
      try 
      {
        SHCmd::setFromRTVal(client, sgCmd);
        RTVal keyVal = RTVal::ConstructString(m_client, "ownerPath");
        RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString ownerPath = QString(nameVal.getStringCString());

        keyVal = RTVal::ConstructString(m_client, "name");
        nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString name = QString(nameVal.getStringCString());
        m_description += QString( "(" + ownerPath + ", " + name + ")" );
      }
      catch(Exception e)
      {
        printf("SGAddPropertyCmd::setFromRTVal: exception: %s\n", e.getDesc_cstr());
      }
    }
};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGADDPROPERTYCMD_H__
