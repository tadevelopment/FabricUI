// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "SetXfoPortDefaultValueCommand.h"
#include <FabricUI/Commands/CommandArgHelpers.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace DFG;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

SetXfoPortDefaultValueCommand::SetXfoPortDefaultValueCommand() 
  : BaseRTValScriptableCommand()
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg(
    "target", 
    "Xfo",
    CommandArgFlags::LOGGABLE_ARG | CommandArgFlags::IO_ARG
    );
  
  // No-optional arg of unknown KL type, which
  // is retrieved when executing the command.
  declareRTValArg(
    "newValue",
    "Xfo",
    CommandArgFlags::LOGGABLE_ARG | CommandArgFlags::IN_ARG
    );

  // Optional arg of unknown KL type, which
  // is retrieved when executing the command.
  declareRTValArg(
    "previousValue", 
    "Xfo",
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

  FABRIC_CATCH_END("SetXfoPortDefaultValueCommand::SetXfoPortDefaultValueCommand");
}

SetXfoPortDefaultValueCommand::~SetXfoPortDefaultValueCommand() 
{
}

bool SetXfoPortDefaultValueCommand::canUndo()
{
  FABRIC_CATCH_BEGIN();
  
  return getRTValArgValue("isUndoable").getBoolean();

  FABRIC_CATCH_END("SetXfoPortDefaultValueCommand::canUndo");

  return false;
}

bool SetXfoPortDefaultValueCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  if(canUndo() && getRTValArgType("previousValue") != "Xfo")
    setRTValArgValue("previousValue", getRTValArgValue("target"));

  setRTValArgValue("target", getRTValArgValue("newValue", "Xfo"));

  return true;

  FABRIC_CATCH_END("SetXfoPortDefaultValueCommand::doIt");

  return false;
} 

bool SetXfoPortDefaultValueCommand::undoIt()
{ 
  FABRIC_CATCH_BEGIN();

  setRTValArgValue("target", getRTValArgValue("previousValue"));
  return true;
  
  FABRIC_CATCH_END("SetXfoPortDefaultValueCommand::undoIt");

  return false;
} 

bool SetXfoPortDefaultValueCommand::redoIt()
{
  return doIt();
} 

QString SetXfoPortDefaultValueCommand::getHelp()
{
  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Absolute path of the Xfo port";
  argsHelp["newValue"] = "New Xfo";
  argsHelp["previousValue"] = "Previous Xfo";
  argsHelp["isUndoable"] = "If true, the command is undoable";

  return CommandArgHelpers::CreateHelpFromRTValArgs(
    "Sets the value of a Xfo DFG port",
    argsHelp,
    this);
}
