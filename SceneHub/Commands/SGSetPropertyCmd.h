/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSETPROPERTYCMD_H__
#define __UI_SCENEHUB_SGSETPROPERTYCMD_H__

#include "SGBaseSetPropertyCmd.h"
using namespace FabricCore;
 
namespace FabricUI {
namespace SceneHub {

class SGSetPropertyCmd : public SGBaseSetPropertyCmd {

  public:   
    SGSetPropertyCmd()
      : SGBaseSetPropertyCmd() {
    }

    SGSetPropertyCmd(
      Client client, 
      RTVal sg, 
      QString propertyPath,
      RTVal prevValue, 
      RTVal newValue)
      : SGBaseSetPropertyCmd() 
    {
      try 
      {
        m_client = client; 
        RTVal args[7] = 
        {
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
        printf("SGSetPropertyCmd::SGSetPropertyCmd: exception: %s\n", e.getDesc_cstr());
      }
    }

    SGSetPropertyCmd(
      Client client, 
      RTVal sg, 
      QString propertyPath, 
      RTVal newValue)
      : SGBaseSetPropertyCmd() 
    {
      try 
      {
        m_client = client; 
        RTVal args[4] = {
          RTVal::ConstructString(m_client, propertyPath.toUtf8().constData()),
          newValue.callMethod("Type", "type", 0, 0),
          newValue.callMethod("Data", "data", 0, 0),
          newValue.callMethod("UInt64", "dataSize", 0, 0),
        };
        sg.callMethod("Cmd", "setPropertyCmd", 4, args);
      }
      catch(FabricCore::Exception e)
      {
        printf("SGSetPropertyCmd::SGSetPropertyCmd: exception: %s\n", e.getDesc_cstr());
      }
    }

    virtual void registerCommand() {
      if(QMetaType::type("SGSetPropertyCmd") == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGSetPropertyCmd>("SGSetPropertyCmd");
    }

};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGSETPROPERTYCMD_H__
