// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "CommandArgFlags.h"
#include "KLCommandManager.h"
#include "CommandArgHelpers.h"
#include "BaseRTValPathValueCommand.h"
#include <FabricUI/Util/FabricException.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>
 
using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
 
BaseRTValPathValueCommand::BaseRTValPathValueCommand() 
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
      "BaseRTValPathValueCommand::BaseRTValPathValueCommand",
      "",
      e.what());
  }
}

BaseRTValPathValueCommand::~BaseRTValPathValueCommand() 
{
}

bool BaseRTValPathValueCommand::canUndo()
{
  try 
  {    
    return getRTValArg("isUndoable").getBoolean();
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValPathValueCommand::canUndo",
      "",
      e.what());
  }

  return false;
}

bool BaseRTValPathValueCommand::doIt()
{
  try
  {
    QString portType = PathValueResolverRegistry::GetRegistry()->getType(
      getRTValArg("target"));

    if(canUndo() && getRTValArgType("previousValue.value") != portType)
      setRTValArg("previousValue", getRTValArg("target.value"));

    RTVal newValue = getRTValArg("newValue.value", portType);
  
    setRTValArg("target.value", newValue);
 
    return true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValPathValueCommand::doIt",
      "",
      e.what());
  }
 
  return false;
} 

bool BaseRTValPathValueCommand::undoIt()
{ 
  try
  {
    setRTValArg("target.value", getRTValArg("previousValue.value"));
    return true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValPathValueCommand::undoIt",
      "",
      e.what());
  }
 
  return false;
} 

bool BaseRTValPathValueCommand::redoIt()
{
  return doIt();
} 

QString BaseRTValPathValueCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;
 
  argsDesc["target"] = getRTValArg("target.path").getStringCString();;
 
  return CreateHistoryDescFromArgs(
    argsDesc,
    this);
}
