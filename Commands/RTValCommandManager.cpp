//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandRegistry.h"
#include "RTValCommandManager.h"
#include <FabricUI/Util/RTValUtil.h>
#include "BaseRTValScriptableCommand.h"
#include <FabricUI/Commands/CommandArgFlags.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Commands/PathValueResolverRegistry.h>

using namespace FabricUI;
using namespace Util;
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
  bool doCmd,
  int interactionID)
{
  try 
  {  
    BaseCommand *cmd = CommandRegistry::GetCommandRegistry()->createCommand(
      cmdName);
    
    cmd->setInteractionID(
      interactionID);

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
  while(ite.hasNext()) 
  {
    ite.next();
    RTVal arg = ite.value();

    if(RTValUtil::getType(arg) == "PathValue")
      scriptCmd->setRTValArg(ite.key(), arg);
    else
      scriptCmd->setRTValArgValue(ite.key(), RTValUtil::toKLRTVal(arg));
  }

  scriptCmd->validateSetArgs();
}

void RTValCommandManager::preDoCommand(
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
      "RTValCommandManager::preDoCommand",
      "",
      e.getDesc_cstr()
      );
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "RTValCommandManager::preDoCommand",
      "",
      e.what());
  }
}

void RTValCommandManager::postDoCommand(
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
      "RTValCommandManager::postDoCommand",
      "",
      e.getDesc_cstr()
      );
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "RTValCommandManager::postDoCommand",
      "",
      e.what());
  }
}
