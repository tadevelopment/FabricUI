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

  FABRIC_CATCH_END("SetPathValueCommand::SetPathValueCommand");
}

SetPathValueCommand::~SetPathValueCommand() 
{
}

bool SetPathValueCommand::canUndo()
{
  return true;
}

bool SetPathValueCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  if( canUndo() )
    m_previousValue = getRTValArgValue("target").clone();

  return redoIt();

  FABRIC_CATCH_END("SetPathValueCommand::doIt");

  return false;
}

bool SetPathValueCommand::undoIt()
{ 
  FABRIC_CATCH_BEGIN();

  setRTValArgValue("target", m_previousValue.clone());
  return true;
  
  FABRIC_CATCH_END("SetPathValueCommand::undoIt");

  return false;
} 

bool SetPathValueCommand::redoIt()
{
  FABRIC_CATCH_BEGIN();

  setRTValArgValue("target", getRTValArgValue("newValue", getRTValArgType("target")).clone());
  return true;

  FABRIC_CATCH_END("SetPathValueCommand::doIt");
 
  return false;
} 

QString SetPathValueCommand::getHelp()
{
  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Path of the target";
  argsHelp["newValue"] = "New value";

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
  BaseCommand *prevCmd) 
{
  SetPathValueCommand *pathValueCmd = qobject_cast<SetPathValueCommand*>(prevCmd);
  m_previousValue = pathValueCmd->m_previousValue;
}
