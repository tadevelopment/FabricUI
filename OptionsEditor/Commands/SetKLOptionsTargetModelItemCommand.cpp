//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "../OptionsEditorHelpers.h"
#include "SetKLOptionsTargetModelItemCommand.h"
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Commands/CommandArgHelpers.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace OptionsEditor;

SetKLOptionsTargetModelItemCommand::SetKLOptionsTargetModelItemCommand() 
  : BaseRTValPathValueCommand()
{
}

SetKLOptionsTargetModelItemCommand::~SetKLOptionsTargetModelItemCommand()
{
}

QString SetKLOptionsTargetModelItemCommand::getHelp()
{
  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Absolute path of the port";
  argsHelp["newValue"] = "New value, must be of the same type than the port.";
  argsHelp["previousValue"] = "Previous value, must be of the same type than the port.";
  argsHelp["isUndoable"] = "If true, the command is undoable.";

  return CreateHelpFromRTValArgs(
    "Sets the value of a KL OptionsEditorTarget model item",
    argsHelp,
    this);
}
