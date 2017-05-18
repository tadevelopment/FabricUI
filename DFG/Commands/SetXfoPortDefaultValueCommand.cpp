// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/Util/RTValUtil.h>
#include "SetXfoPortDefaultValueCommand.h"
#include <FabricUI/Commands/CommandArgFlags.h>
#include <FabricUI/Commands/CommandException.h>
#include <FabricUI/Commands/KLCommandManager.h>
 #include <FabricUI/Commands/CommandArgHelpers.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
 
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

    KLCommandManager *manager = dynamic_cast<KLCommandManager*>(
      CommandManager::GetCommandManager());
    
    // Optional arg of known KL type, with default value.                  
    declareRTValArg( 
      "isUndoable",
      "Boolean",
      CommandArgFlags::OPTIONAL_ARG,
      RTVal::ConstructBoolean(
        manager->getClient(), 
        true)
      );
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
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
    return getRTValArg("isUndoable").getBoolean();
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
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
    if(canUndo() && RTValUtil::getRTValType(getRTValArg("previousValue.value")) != "Xfo")
      setRTValArg("previousValue", getRTValArg("target.value"));

    RTVal newValue = getRTValArg("newValue.value", "Xfo");
  
    setRTValArg("target.value", newValue);
 
    return true;
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
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
    setRTValArg("target.value", getRTValArg("previousValue.value"));
    return true;
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
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
  argsHelp["newValue"] = "New value";
  argsHelp["previousValue"] = "Previous value";
  argsHelp["isUndoable"] = "If true, the command is undoable";

  return CreateHelpFromRTValArgs(
    "Sets the value of a Xfo DFG port",
    argsHelp,
    this);
}

QString SetXfoPortDefaultValueCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;
 
  argsDesc["target"] = getRTValArg("target.path").getStringCString();;
 
  return CreateHistoryDescFromArgs(
    argsDesc,
    this);
}
