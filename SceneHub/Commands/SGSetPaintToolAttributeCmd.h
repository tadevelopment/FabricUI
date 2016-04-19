/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSETPAINTTOOLATTRIBUTECMD_H__
#define __UI_SCENEHUB_SGSETPAINTTOOLATTRIBUTECMD_H__
 
#include "SHCmd.h"
using namespace FabricCore;
 
namespace FabricUI {
namespace SceneHub {

class SGSetPaintToolAttributeCmd : public SHCmd {

  public: 
    SGSetPaintToolAttributeCmd() : SHCmd() {}

    virtual void registerCommand() {
      if(QMetaType::type("SGSetPaintToolAttributeCmd") == 0)
        qRegisterMetaType<FabricUI::SceneHub::SGSetPaintToolAttributeCmd>("SGSetPaintToolAttributeCmd");
    }

    virtual void setFromRTVal(Client client, RTVal sgCmd) {
      try 
      {
        m_client = client;
        RTVal keyVal = RTVal::ConstructString(m_client, "fullPath");
        RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
        QString fullPath = QString(fullPathVal.getStringCString());
        m_description = QString( "setPaintToolAttributeCmd(" + fullPath + ")" );
      }
      catch(Exception e)
      {
        printf("SGSetPaintToolAttributeCmd::Get: exception: %s\n", e.getDesc_cstr());
      }
    }
};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_SGSETPAINTTOOLATTRIBUTECMD_H__
