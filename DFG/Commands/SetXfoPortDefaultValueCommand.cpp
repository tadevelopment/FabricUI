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
  try
  {
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
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetXfoPortDefaultValueCommand::SetXfoPortDefaultValueCommand",
      "",
      e.what());
  }
}

SetXfoPortDefaultValueCommand::~SetXfoPortDefaultValueCommand() 
{
}

bool SetXfoPortDefaultValueCommand::canUndo()
{
  try 
  {    
    return getRTValArgValue("isUndoable").getBoolean();
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetXfoPortDefaultValueCommand::canUndo",
      "",
      e.what());
  }

  return false;
}

bool SetXfoPortDefaultValueCommand::doIt()
{
  try
  {
    if(canUndo() && getRTValArgType("previousValue") != "Xfo")
      setRTValArgValue("previousValue", getRTValArgValue("target"));

    RTVal newValue = getRTValArgValue("newValue", "Xfo");
  
    setRTValArgValue("target", newValue);
 
    return true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetXfoPortDefaultValueCommand::doIt",
      "",
      e.what());
  }
 
  return false;
} 

bool SetXfoPortDefaultValueCommand::undoIt()
{ 
  try
  {
    setRTValArgValue("target", getRTValArgValue("previousValue"));
    return true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetXfoPortDefaultValueCommand::undoIt",
      "",
      e.what());
  }
 
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
