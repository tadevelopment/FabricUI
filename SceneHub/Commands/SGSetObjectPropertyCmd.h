/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSETObjectPROPERTYCMD_H__
#define __UI_SCENEHUB_SGSETObjectPROPERTYCMD_H__

#include "SGBaseSetPropertyCmd.h"
using namespace FabricCore;
 
namespace FabricUI {
namespace SceneHub {

class SGSetObjectPropertyCmd : public SGBaseSetPropertyCmd {

  public:   
    SGSetObjectPropertyCmd()
      : SGBaseSetPropertyCmd() {
      }

    SGSetObjectPropertyCmd(
      Client client, 
      RTVal sg, 
      QString 
      propertyPath, 
      RTVal prevValue, 
      RTVal newValue)
      : SGBaseSetPropertyCmd() 
    {
      try 
      {
        m_client = client; 
        if(newValue.isObject())
        {
          RTVal args[3] = {
            RTVal::ConstructString(m_client, propertyPath.toUtf8().constData()),
            prevValue,
            newValue
          };
          sg.callMethod("Cmd", "setPropertyCmd", 3, args);
        }
      }
      catch(FabricCore::Exception e)
      {
        printf("SGSetObjectPropertyCmd::SGSetObjectPropertyCmd: exception: %s\n", e.getDesc_cstr());
      }
    }

    SGSetObjectPropertyCmd(
      Client client, 
      RTVal sg, 
      QString propertyPath, 
      RTVal newValue)
      : SGBaseSetPropertyCmd() 
    {
      try 
      {
        m_client = client; 
        if(newValue.isObject())
        {
          RTVal args[2] = {
            RTVal::ConstructString(m_client, propertyPath.toUtf8().constData()),
            newValue
          };
          sg.callMethod("Cmd", "setPropertyCmd", 2, args);
        }

      }
      catch(FabricCore::Exception e)
      {
        printf("SGSetObjectPropertyCmd::SGSetObjectPropertyCmd: exception: %s\n", e.getDesc_cstr());
      }
    }

    virtual void registerCommand() {
      if(QMetaType::type("SGSetBooleanPropertyCmd") == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGSetBooleanPropertyCmd>("SGSetBooleanPropertyCmd");
    }   

};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGSETObjectPROPERTYCMD_H__
