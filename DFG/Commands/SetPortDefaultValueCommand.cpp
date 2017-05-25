// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "SetPortDefaultValueCommand.h"
#include <FabricUI/Util/FabricException.h>
#include <FabricUI/Commands/CommandArgHelpers.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
 
SetPortDefaultValueCommand::SetPortDefaultValueCommand() 
  : BaseRTValPathValueCommand()
{
}

SetPortDefaultValueCommand::~SetPortDefaultValueCommand() 
{
}

QString SetPortDefaultValueCommand::getHelp()
{
  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Absolute path of the port";
  argsHelp["newValue"] = "New value, must be of the same type than the port.";
  argsHelp["previousValue"] = "Previous value, must be of the same type than the port.";
  argsHelp["isUndoable"] = "If true, the command is undoable.";

  return CreateHelpFromRTValArgs(
    "Sets the value of a DFG port",
    argsHelp,
    this);
}
