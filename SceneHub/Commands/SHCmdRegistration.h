/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_REGISTRATION_H__
#define __UI_SCENEHUB_CMD_REGISTRATION_H__

#include "SGAddObjectCmd.h"
#include "SGAddPropertyCmd.h"
#include "SGSetPropertyCmd.h"
#include "SGSetPaintToolAttributeCmd.h"

namespace FabricUI {
namespace SceneHub {

class SHCmdRegistration  {

  public:
    SHCmdRegistration() { 
      SGAddObjectCmd addObjectCmd;
      addObjectCmd.registerCommand();

      SGAddPropertyCmd addPropertyCmd;
      addPropertyCmd.registerCommand();

      SGSetPropertyCmd setPropertyCmd;
      setPropertyCmd.registerCommand();
 
      SGSetPaintToolAttributeCmd setPaintToolAttributeCmd;
      setPaintToolAttributeCmd.registerCommand();
    }

    ~SHCmdRegistration() {}
};

} // SceneHub
} // FabricUI 

#endif // __UI_SCENEHUB_CMD_REGISTRATION_H__
