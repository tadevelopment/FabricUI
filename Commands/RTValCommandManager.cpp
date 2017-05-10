//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandException.h"
#include "CommandRegistry.h"
#include "RTValCommandManager.h"
#include "ScriptableCommand.h"
#include "RTValScriptableCommand.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

RTValCommandManager::RTValCommandManager(
  Client client) 
  : CommandManager()
  , m_client(client)
{
}

RTValCommandManager::~RTValCommandManager() 
{
}

FabricCore::Client RTValCommandManager::getClient()
{
  return m_client;
}

Command* RTValCommandManager::createCommand(
  const QString &cmdName, 
  const QMap<QString, RTVal> &args, 
  bool doCmd)
{
  try 
  {  
    Command *cmd = CommandRegistry::GetCommandRegistry()->createCommand(
      cmdName);
    
    if(args.size() > 0) 
      checkCommandArgs(cmd, args);

    if(doCmd) 
      doCommand(cmd);

    return cmd;
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "RTValCommandManager::createCommand",
      "Cannot create command '" + cmdName + "'",
      e.what()
      );
  }

  catch(Exception &e) 
  {
    CommandException::PrintOrThrow(
      "RTValCommandManager::createCommand",
      "Cannot create command '" + cmdName + "'",
      e.getDesc_cstr()
      );
  }

  return 0;
}

void RTValCommandManager::doCommand(
  Command *cmd) 
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
  Command *cmd,
  const QMap<QString, RTVal> &args)
{ 
  RTValScriptableCommand* rtvalScriptCommand = dynamic_cast<RTValScriptableCommand*>(cmd);
  
  if(!rtvalScriptCommand) 
    CommandException::PrintOrThrow(
      "RTValCommandManager::checkCommandArgs",

      "Command '" + cmd->getName() + "' is created with args, " + 
      "but is not implementing the RTValScriptableCommand interface"
      );

  // Sets the rtval args
  QMapIterator<QString, RTVal> ite(args);
  while (ite.hasNext()) 
  {
    ite.next();

    rtvalScriptCommand->setRTValArg(
      ite.key(), 
      ite.value());
  }

  ScriptableCommand* scriptCommand = dynamic_cast<ScriptableCommand*>(cmd);
  scriptCommand->validateSetArgs();
}
