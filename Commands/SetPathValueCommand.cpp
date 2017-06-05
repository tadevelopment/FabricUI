// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "CommandArgHelpers.h"
#include "CommandArgHelpers.h"
#include "SetPathValueCommand.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

SetPathValueCommand::SetPathValueCommand() 
  : BaseRTValScriptableCommand()
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg(
    "target", 
    "RTVal",
    CommandArgFlags::LOGGABLE_ARG | CommandArgFlags::IO_ARG
    );
  
  // No-optional arg of unknown KL type, which
  // is retrieved when executing the command.
  declareRTValArg(
    "newValue",
    "RTVal",
    CommandArgFlags::LOGGABLE_ARG | CommandArgFlags::IN_ARG
    );

  // Optional arg of unknown KL type, which
  // is retrieved when executing the command.
  declareRTValArg(
    "previousValue", 
    "RTVal",
    CommandArgFlags::OPTIONAL_ARG | CommandArgFlags::IN_ARG
    );

  // Optional arg of known KL type, with default value.                  
  declareRTValArg( 
    "isUndoable",
    "Boolean",
    CommandArgFlags::OPTIONAL_ARG,
    RTVal::ConstructBoolean(
      FabricApplicationStates::GetAppStates()->getContext(), 
      true)
    );
  
  FABRIC_CATCH_END("SetPathValueCommand::SetPathValueCommand");
}

SetPathValueCommand::~SetPathValueCommand() 
{
}

bool SetPathValueCommand::canUndo()
{
  FABRIC_CATCH_BEGIN();
   
  return getRTValArgValue("isUndoable").getBoolean() ||
    getCanMergeID() > -1;

  FABRIC_CATCH_END("SetPathValueCommand::canUndo");

  return false;
}

bool SetPathValueCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  if(canUndo() && getRTValArgType("previousValue") != getRTValArgType("target"))
    setRTValArgValue("previousValue", getRTValArgValue("target").clone());

  setRTValArgValue("target", getRTValArgValue("newValue", getRTValArgType("target")).clone());
  return true;

  FABRIC_CATCH_END("SetPathValueCommand::doIt");
 
  return false;
} 

bool SetPathValueCommand::undoIt()
{ 
  FABRIC_CATCH_BEGIN();

  setRTValArgValue("target", getRTValArgValue("previousValue").clone());
  return true;
  
  FABRIC_CATCH_END("SetPathValueCommand::undoIt");

  return false;
} 

bool SetPathValueCommand::redoIt()
{
  return doIt();
} 

QString SetPathValueCommand::getHelp()
{
  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Path of the target";
  argsHelp["newValue"] = "New value";
  argsHelp["previousValue"] = "Previous value";
  argsHelp["isUndoable"] = "If true, the command is undoable";

  return CommandArgHelpers::CreateHelpFromRTValArgs(
    "Sets the value of a PathValue arg",
    argsHelp,
    this);
}

QString SetPathValueCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;
  argsDesc["target"] = getRTValArgPath("target");
  return CommandArgHelpers::CreateHistoryDescFromArgs(
    argsDesc,
    this);
}

void SetPathValueCommand::merge(
  BaseCommand *cmd) 
{
  FABRIC_CATCH_BEGIN();

  SetPathValueCommand *pathValueCmd = qobject_cast<SetPathValueCommand*>(cmd);
  setRTValArgValue("newValue", pathValueCmd->getRTValArgValue("newValue").clone());

  FABRIC_CATCH_END("SetPathValueCommand::merge");
}
