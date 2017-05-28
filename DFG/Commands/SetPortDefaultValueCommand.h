// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_SET_PORT_DEFAULT_VALUE_COMMAND__
#define __UI_SET_PORT_DEFAULT_VALUE_COMMAND__

#include <FabricUI/Commands/SetPathValueCommand.h>

namespace FabricUI {
namespace DFG {

class SetPortDefaultValueCommand : public Commands::SetPathValueCommand
{
  Q_OBJECT

  public:
    SetPortDefaultValueCommand();

    virtual ~SetPortDefaultValueCommand();

    /// Implementation of Command.
    virtual QString getHelp();
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_SET_PORT_DEFAULT_VALUE_COMMAND__
