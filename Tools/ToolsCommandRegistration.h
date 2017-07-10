//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_TOOLS_COMMAND_REGISTRATION__
#define __UI_TOOLS_COMMAND_REGISTRATION__

#include "ToolsNotifier.h"
#include "CreateToolCommand.h"
#include <FabricUI/Commands/CommandRegistry.h>
 
namespace FabricUI {
namespace Tools {

class ToolsCommandRegistration
{
  /**
    ToolsCommandRegistration registered all the Tools commands. 
  */

  public:
   	static void RegisterCommands(
   		ToolsNotifierRegistry *registry)
    {
      Commands::CommandFactory<CreateToolCommand>::Register(
        "createTool",
        (void*)registry
        );
    }
};

} // namespace Tools 
} // namespace FabricUI

#endif // __UI_TOOLS_COMMAND_REGISTRATION__
