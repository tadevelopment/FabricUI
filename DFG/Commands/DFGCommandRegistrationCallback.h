//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_COMMAND_REGISTRY_CALLBACK__
#define __UI_DFG_COMMAND_REGISTRY_CALLBACK__

#include "SetPortDefaultValueCommand.h"
#include "SetXfoPortDefaultValueCommand.h"
#include "SetPortsDefaultValuesCommand.h"
#include <FabricUI/Commands/CommandRegistry.h>

namespace FabricUI {
namespace DFG {

class DFGCommandRegistrationCallback
{
  /**
    DFGCommandRegistrationCallback registered the commands for the DFG.
  */

  public:
    DFGCommandRegistrationCallback()
    {
    }

    static void RegisterCommands()
    {
      Commands::CommandFactory<SetPortDefaultValueCommand>::Register(
        "setPortDefaultValue");

      Commands::CommandFactory<SetXfoPortDefaultValueCommand>::Register(
        "setXfoPortDefaultValue");

      Commands::CommandFactory<SetPortsDefaultValuesCommand>::Register(
        "setPortsDefaultValues");
    }
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_COMMAND_REGISTRY_CALLBACK__
