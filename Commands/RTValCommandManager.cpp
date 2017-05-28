//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <iostream>
#include "CommandRegistry.h"
#include "RTValCommandManager.h"
#include "BaseRTValScriptableCommand.h"
#include <FabricUI/Commands/CommandArgFlags.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;
 
RTValCommandManager::RTValCommandManager() 
  : CommandManager()
{
}

RTValCommandManager::~RTValCommandManager() 
{
}
 
BaseCommand* RTValCommandManager::createCommand(
  const QString &cmdName, 
  const QMap<QString, RTVal> &args, 
  bool doCmd)
{
  try 
  {  
    BaseCommand *cmd = CommandRegistry::GetCommandRegistry()->createCommand(
      cmdName);
    
    if(args.size() > 0) 
      checkCommandArgs(cmd, args);

    if(doCmd) 
      doCommand(cmd);

    return cmd;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "RTValCommandManager::createCommand",
      "Cannot create command '" + cmdName + "'",
      e.what()
      );
  }
 
  return 0;
}

void RTValCommandManager::doCommand(
  BaseCommand *cmd) 
{
  try
  {
    CommandManager::doCommand(
      cmd);
  }
   
  catch(Exception &e) 
  {
    cleanupUnfinishedCommandsAndThrow(
      cmd,
      e.getDesc_cstr()
      );
  }
}

void RTValCommandManager::checkCommandArgs(
  BaseCommand *cmd,
  const QMap<QString, RTVal> &args)
{ 
  BaseRTValScriptableCommand* scriptCmd = qobject_cast<BaseRTValScriptableCommand*>(cmd);

  if(!scriptCmd) 
    FabricException::Throw(
      "RTValCommandManager::checkCommandArgs",
      "BaseCommand '" + cmd->getName() + "' is created with args, " + 
      "but is not implementing the BaseRTValScriptableCommand interface"
      );

  // Sets the rtval args
  QMapIterator<QString, RTVal> ite(args);
  while (ite.hasNext()) 
  {
    ite.next();

    scriptCmd->setRTValArg(
      ite.key(), 
      ite.value());
  }

  scriptCmd->validateSetArgs();
}

void RTValCommandManager::preProcessCommandArgs(
  BaseCommand* cmd)
{
  BaseRTValScriptableCommand* scriptCmd = qobject_cast<BaseRTValScriptableCommand*>(cmd);
  if(!scriptCmd) return;

  try
  {
    QString key;
    foreach(key, scriptCmd->getArgKeys())
    {
      if( scriptCmd->isArg(key, CommandArgFlags::IN_ARG) ||
          scriptCmd->isArg(key, CommandArgFlags::IO_ARG) )
      {
        RTVal pathValue = scriptCmd->getRTValArg(key);
        if(PathValueResolverRegistry::GetRegistry()->knownPath(pathValue))
        {
          PathValueResolverRegistry::GetRegistry()->getValue(pathValue);
          scriptCmd->setRTValArg(key, pathValue);
        }
      }
    }
  }
   
  catch(Exception &e) 
  {
    FabricException::Throw(
      "RTValCommandManager::preProcessCommandArgs",
      "",
      e.getDesc_cstr()
      );
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "RTValCommandManager::preProcessCommandArgs",
      "",
      e.what());
  }
}

void RTValCommandManager::postProcessCommandArgs(
  BaseCommand* cmd)
{
  BaseRTValScriptableCommand* scriptCmd = qobject_cast<BaseRTValScriptableCommand*>(cmd);
  if(!scriptCmd) return;

  try
  {
    QString key;
    foreach(key, scriptCmd->getArgKeys())
    {         
      if( scriptCmd->isArg(key, CommandArgFlags::OUT_ARG) ||
          scriptCmd->isArg(key, CommandArgFlags::IO_ARG) )
      {
        RTVal pathValue = scriptCmd->getRTValArg(key);
        if(PathValueResolverRegistry::GetRegistry()->knownPath(pathValue))
          PathValueResolverRegistry::GetRegistry()->setValue(
            pathValue);
      }
    }
  }
   
  catch(Exception &e) 
  {
    FabricException::Throw(
      "RTValCommandManager::postProcessCommandArgs",
      "",
      e.getDesc_cstr()
      );
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "RTValCommandManager::postProcessCommandArgs",
      "",
      e.what());
  }
}
