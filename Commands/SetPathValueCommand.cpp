// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <iostream>
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
  try
  {
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
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPathValueCommand::SetPathValueCommand",
      "",
      e.what());
  }
}

SetPathValueCommand::~SetPathValueCommand() 
{
}

bool SetPathValueCommand::canUndo()
{
  try 
  {    
    return getRTValArgValue("isUndoable").getBoolean() ||
      getInteractionID() > -1;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPathValueCommand::canUndo",
      "",
      e.what());
  }

  return false;
}

bool SetPathValueCommand::doIt()
{
  try
  {
    if(canUndo() && getRTValArgType("previousValue") != getRTValArgType("target"))
      setRTValArgValue("previousValue", getRTValArgValue("target").clone());
  
    setRTValArgValue("target", getRTValArgValue("newValue", getRTValArgType("target")).clone());
    
    return true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPathValueCommand::doIt",
      "",
      e.what());
  }
 
  return false;
} 

bool SetPathValueCommand::undoIt()
{ 
  try
  {
    setRTValArgValue("target", getRTValArgValue("previousValue").clone());
    return true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPathValueCommand::undoIt",
      "",
      e.what());
  }
 
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
  SetPathValueCommand *pathValueCmd = qobject_cast<SetPathValueCommand*>(cmd);
  setRTValArgValue("newValue", pathValueCmd->getRTValArgValue("newValue").clone());
}
