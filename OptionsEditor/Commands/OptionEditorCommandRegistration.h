//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_EDITOR_COMMAND_REGISTRATION__
#define __UI_OPTIONS_EDITOR_COMMAND_REGISTRATION__

#include "SetOptionsModelValueCommand.h"
#include "CloseOptionsEditorCommand.h"
#include "OpenOptionsEditorCommand.h"
#include <FabricUI/Commands/CommandRegistry.h>

namespace FabricUI {
namespace OptionsEditor {

class OptionEditorCommandRegistration
{
  /**
    OptionEditorCommandRegistration registered all the OptionsEditor commands. 
  */

  public:
    OptionEditorCommandRegistration()
    {
    }

   	static void RegisterCommands()
    {
      Commands::CommandFactory<SetOptionsModelValueCommand>::Register(
        "setOptionsModelValue");

      Commands::CommandFactory<OpenOptionsEditorCommand>::Register(
        "openOptionsEditor");

      Commands::CommandFactory<CloseOptionsEditorCommand>::Register(
        "closeOptionsEditor");
    }
};


} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPTIONS_EDITOR_COMMAND_REGISTRATION__
