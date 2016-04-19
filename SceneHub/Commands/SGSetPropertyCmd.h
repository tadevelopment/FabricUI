/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSETPROPERTYCMD_H__
#define __UI_SCENEHUB_SGSETPROPERTYCMD_H__

#include "SHCmd.h"
using namespace FabricCore;
 
namespace FabricUI {
namespace SceneHub {


class SGSetPropertyCmd : public SHCmd {

  public:   
    SGSetPropertyCmd(): SHCmd() {}

    SGSetPropertyCmd(
      Client client, 
      RTVal sg, 
      QString propertyPath, 
      RTVal prevValue,
      RTVal newValue)
    : SHCmd() 
    {
      try 
      {
        m_client = client; 
        RTVal args[7] = {
          RTVal::ConstructString(m_client, propertyPath.toUtf8().constData()),
          prevValue.callMethod("Type", "type", 0, 0),
          prevValue.callMethod("Data", "data", 0, 0),
          prevValue.callMethod("UInt64", "dataSize", 0, 0),
          newValue.callMethod("Type", "type", 0, 0),
          newValue.callMethod("Data", "data", 0, 0),
          newValue.callMethod("UInt64", "dataSize", 0, 0),
        };
        sg.callMethod("Cmd", "setPropertyCmd", 7, args);

      }
      catch(FabricCore::Exception e)
      {
        printf("SGSetPropertyCmd::Get: exception: %s\n", e.getDesc_cstr());
      }
    }

    virtual void registerCommand() {
      if(QMetaType::type("SGSetPropertyCmd") == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGSetPropertyCmd>("SGSetPropertyCmd");
    }

    virtual void setFromRTVal(Client client, RTVal sgCmd) {
      try 
      {
        m_client = client;
        RTVal keyVal = RTVal::ConstructString(m_client, "fullPath");
        RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString fullPath = QString(fullPathVal.getStringCString());
        m_description = QString( "setPropertyCmd(" + fullPath + ")" );   
      }
      catch(FabricCore::Exception e)
      {
        printf("SGSetPropertyCmd::Get: exception: %s\n", e.getDesc_cstr());
      }
    }    

};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGSETPROPERTYCMD_H__
