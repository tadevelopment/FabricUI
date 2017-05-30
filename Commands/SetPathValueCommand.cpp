// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <iostream>
#include "CommandArgFlags.h"
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
    RTVal val = getRTValArgValue("isUndoable");
    std::cout 
      << "SetPathValueCommand::canUndo "
      << RTValUtil::getType(val).toUtf8().constData()
      << std::endl;

    return val.getBoolean();
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
    RTVal target = getRTValArg("target");

    std::cout 
      << "SetPathValueCommand::doIt 1 "
      << RTValUtil::getType(target).toUtf8().constData()
      << " "
      << getRTValArgPath("target").toUtf8().constData()
      << std::endl;
   
    QString dataType = PathValueResolverRegistry::GetRegistry()->getType(
      target);

    std::cout 
      << "SetPathValueCommand::doIt 2 "
      << dataType.toUtf8().constData()
      << std::endl;
    
    if(canUndo() && getRTValArgType("previousValue") != dataType)
      setRTValArgValue("previousValue", getRTValArgValue("target").clone());
 
     std::cout 
      << "SetPathValueCommand::doIt 3 "
      << std::endl;
    
    setRTValArgValue("target", getRTValArgValue("newValue", dataType).clone());
    
    std::cout 
      << "SetPathValueCommand::doIt 4 "
      << std::endl;

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
 
  argsDesc["target"] = "<" + getRTValArgPath("target") + ">";
 
  return CommandArgHelpers::CreateHistoryDescFromArgs(
    argsDesc,
    this);
}
