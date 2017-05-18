// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "SetPortDefaultValueCommand.h"
#include <FabricUI/Commands/CommandArgFlags.h>
#include <FabricUI/Util/FabricException.h>
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/PathResolvers/PathResolver.h>
#include <FabricUI/Commands/CommandArgHelpers.h>
 
using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace PathResolvers;

SetPortDefaultValueCommand::SetPortDefaultValueCommand() 
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

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPortDefaultValueCommand::SetPortDefaultValueCommand",
      "",
      e.what());
  }
}

SetPortDefaultValueCommand::~SetPortDefaultValueCommand() 
{
}

bool SetPortDefaultValueCommand::canUndo()
{
  try 
  {    
    return getRTValArg("isUndoable").getBoolean();
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPortDefaultValueCommand::canUndo",
      "",
      e.what());
  }

  return false;
}

bool SetPortDefaultValueCommand::doIt()
{
  try
  {
    QString portType = PathResolver::GetPathResolver()->getType(getRTValArg("target"));

    if(canUndo() && getRTValArgType("previousValue.value") != portType)
      setRTValArg("previousValue", getRTValArg("target.value"));

    RTVal newValue = getRTValArg("newValue.value", portType);
  
    setRTValArg("target.value", newValue);
 
    return true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPortDefaultValueCommand::doIt",
      "",
      e.what());
  }
 
  return false;
} 

bool SetPortDefaultValueCommand::undoIt()
{ 
  try
  {
    setRTValArg("target.value", getRTValArg("previousValue.value"));
    return true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "SetPortDefaultValueCommand::undoIt",
      "",
      e.what());
  }
 
  return false;
} 

bool SetPortDefaultValueCommand::redoIt()
{
  return doIt();
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

QString SetPortDefaultValueCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;
 
  argsDesc["target"] = getRTValArg("target.path").getStringCString();;
 
  return CreateHistoryDescFromArgs(
    argsDesc,
    this);
}
