//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_EDITOR_COMMAND_REGISTRATION__
#define __UI_OPTIONS_EDITOR_COMMAND_REGISTRATION__

#include "OpenKLOptionsTargetEditorCommand.h"
#include "CloseKLOptionsTargetEditorCommand.h"
#include "SetKLOptionsTargetModelItemCommand.h"
#include <FabricUI/Commands/CommandRegistry.h>
#include "../PathValueResolvers/OptionsPathValueResolver.h"
#include <FabricUI/PathValueResolvers/PathValueResolverRegistry.h>

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
      OptionsPathValueResolver *resolver = dynamic_cast<OptionsPathValueResolver*>(
        PathValueResolvers::PathValueResolverRegistry::GetRegistry()->getResolver(
          "OptionsPathValueResolver")
        );

      if(resolver == 0)
      {
        resolver = new OptionsPathValueResolver();
        PathValueResolvers::PathValueResolverRegistry::GetRegistry()->registerResolver(
          "OptionsPathValueResolver",
          resolver
          );
      }

      Commands::CommandFactory<SetKLOptionsTargetModelItemCommand>::Register(
        "setKLOptionsTargetModelItem");

      Commands::CommandFactory<OpenKLOptionsTargetEditorCommand>::Register(
        "openKLOptionsTargetEditor");

      Commands::CommandFactory<CloseKLOptionsTargetEditorCommand>::Register(
        "closeKLOptionsTargetEditor");
    }
};


} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPTIONS_EDITOR_COMMAND_REGISTRATION__
