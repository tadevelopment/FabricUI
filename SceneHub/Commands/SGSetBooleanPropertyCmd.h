/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSETBOOLEANPROPERTYCMD_H__
#define __UI_SCENEHUB_SGSETBOOLEANPROPERTYCMD_H__

#include "SGBaseSetPropertyCmd.h"
using namespace FabricCore;
 
namespace FabricUI {
namespace SceneHub {

class SGSetBooleanPropertyCmd : public SGBaseSetPropertyCmd {

  public:   
    SGSetBooleanPropertyCmd()
      : SGBaseSetPropertyCmd() {
    }

    SGSetBooleanPropertyCmd(
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
        RTVal args[3] = {
          RTVal::ConstructString(m_client, propertyPath.toUtf8().constData()),
          prevValue,
          newValue
        };
        sg.callMethod("Cmd", "setPropertyCmd", 3, args);
      }
      catch(FabricCore::Exception e)
      {
        printf("SGSetBooleanPropertyCmd::SGSetBooleanPropertyCmd: exception: %s\n", e.getDesc_cstr());
      }
    }

    SGSetBooleanPropertyCmd(
      Client client, 
      RTVal sg, 
      QString propertyPath, 
      RTVal newValue)
      : SGBaseSetPropertyCmd() 
    {
      try 
      {
        m_client = client; 
        RTVal args[2] = {
          RTVal::ConstructString(m_client, propertyPath.toUtf8().constData()),
          newValue
        };
        sg.callMethod("Cmd", "setPropertyCmd", 2, args);
      }
      catch(FabricCore::Exception e)
      {
        printf("SGSetBooleanPropertyCmd::SGSetBooleanPropertyCmd: exception: %s\n", e.getDesc_cstr());
      }
    }

    virtual void registerCommand() {
      if(QMetaType::type("SGSetBooleanPropertyCmd") == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGSetBooleanPropertyCmd>("SGSetBooleanPropertyCmd");
    }   

};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGSETBOOLEANPROPERTYCMD_H__
