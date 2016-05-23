/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGBASESETPROPERTYCMD_H__
#define __UI_SCENEHUB_SGBASESETPROPERTYCMD_H__

#include "SHCmd.h"
using namespace FabricCore;
 
namespace FabricUI {
namespace SceneHub {


class SGBaseSetPropertyCmd : public SHCmd {

  public:   
    SGBaseSetPropertyCmd()
      : SHCmd() {
    }

    virtual void setFromRTVal(Client client, RTVal sgCmd) {
      try 
      {
        SHCmd::setFromRTVal(client, sgCmd);
        RTVal keyVal = RTVal::ConstructString(m_client, "fullPath");
        RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString fullPath = QString(fullPathVal.getStringCString());
        m_description += QString( "(" + fullPath + ")" );   
      }
      catch(FabricCore::Exception e)
      {
        printf("SGBaseSetPropertyCmd::setFromRTVal: exception: %s\n", e.getDesc_cstr());
      }
    }    

};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGBASESETPROPERTYCMD_H__
