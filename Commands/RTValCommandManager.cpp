//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandHelpers.h"
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

Command* RTValCommandManager::createRTValCommand(
  const QString &cmdName, 
  const QMap<QString, RTVal> &args, 
  bool doCmd)
{
  try 
  {  
    Command *cmd = CommandRegistry::GetCommandRegistry()->createCommand(
      cmdName);
    
    if(args.size() > 0) 
      checkRTValCommandArgs(cmd, args);

    if(doCmd) 
      doCommand(cmd);

    return cmd;
  }

  catch(std::string &e) 
  {
    printAndThrow(
      QString(
        "RTValCommandManager::createRTValCommand, error: Cannot create command '" + 
        cmdName + "', " + 
        QString(e.c_str())
      ).toUtf8().constData()
    );
  }

  return 0;
}

void RTValCommandManager::checkRTValCommandArgs(
  Command *cmd,
  const QMap<QString, RTVal> &args)
{ 
  RTValScriptableCommand* rtvalScriptCommand = dynamic_cast<RTValScriptableCommand*>(cmd);
  
  if(!rtvalScriptCommand) 
    printAndThrow(
      QString(
        "RTValCommandManager::checkRTValCommandArgs, command '" + cmd->getName() + 
        "' is created with args, but is not implementing the RTValScriptableCommand interface"
      ).toUtf8().constData() 
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
