//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_COMMAND_REGISTRY_CALLBACK__
#define __UI_DFG_COMMAND_REGISTRY_CALLBACK__

#include "SetPortDefaultValueCommand.h"
#include <FabricUI/Commands/CommandFactory.h>

namespace FabricUI {
namespace DFG {

class DFGCommandRegistrationCallback : public QObject
{
  /**
    DFGCommandRegistrationCallback registered all
    the BaseDFGCommand commands. It must be called once.
  */

  Q_OBJECT

  public:
    DFGCommandRegistrationCallback()
    {
    }

    /// Registers the commands.
    /// \param dfgController A pointer to the DFGController.
    static void RegisterCommands(
      void *dfgController)
    {
      Commands::CommandFactory<SetPortDefaultValueCommand>::RegisterCommand(
        "setPortDefaultValue",
        (void *)dfgController);
    }
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_COMMAND_REGISTRY_CALLBACK__
