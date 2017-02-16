//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include "CommandManager.h"
#include "CommandRegistry.h"

using namespace FabricUI;
using namespace Commands;

CommandManager::CommandManager() 
  : BaseCommandManager() 
{
}

CommandManager::~CommandManager() 
{
}

BaseCommandManager* CommandManager::GetCommandManager()
{
  BaseCommandManager *manager = BaseCommandManager::GetCommandManager();
  if (!manager)
  {
    manager = new CommandManager();
    BaseCommandManager::SetCommandManagerSingleton(manager);
  }
  
  return manager;
}

BaseCommand* CommandManager::createCommand(
  const QString &cmdName, 
  const QMap<QString, QString> &args, 
  bool doCmd)
{
  try 
  {  
    BaseCommand *cmd = CommandRegistry::GetCommandRegistry()->createCommand(cmdName);
    
    if( args.size() > 0 ) 
      checkCommandArgs(cmd, args);

    if (doCmd) 
      this->doCommand(cmd);
  
    return cmd;
  }

  catch(std::string &e) {
    throw(
      std::string(
        QString("Cannot create command '" + cmdName + "', " + QString(e.c_str())).toUtf8().constData()
      )
    );
  }

  return 0;
}
