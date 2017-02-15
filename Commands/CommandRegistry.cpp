//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include <iostream>
#include "CommandFactory.h"
#include "CommandRegistry.h"

using namespace FabricUI;
using namespace Commands;

//QMap<QString, BaseCommandFactory*> CommandRegistry::s_registeredCmds;
CommandRegistry CommandRegistry::s_cmdRegistry;

CommandRegistry::CommandRegistry() 
{
}

CommandRegistry::~CommandRegistry() 
{
}

void CommandRegistry::RegisterFactory(
  const QString &cmdName, 
  BaseCommandFactory *factory) 
{
  QString factoryType;
  if (!IsCommandRegistered(cmdName, factoryType))
  {
    s_cmdRegistry.m_registeredCmds[cmdName] = factory;
    s_cmdRegistry.commandIsRegistered(cmdName);
  }
}

bool CommandRegistry::IsCommandRegistered(
  const QString &cmdName,
  QString &factoryType) 
{
  bool isRegistered = s_cmdRegistry.m_registeredCmds.count(cmdName) != 0;
  factoryType = isRegistered ? typeid(*s_cmdRegistry.m_registeredCmds[cmdName]).name() : "";
  return isRegistered;
}

BaseCommand* CommandRegistry::CreateCommand(
  const QString &cmdName) 
{
  QString factoryType;
  if (!IsCommandRegistered(cmdName, factoryType))
    throw( 
      std::string(
        QString(
          "Cannot create command, '" + cmdName + "' is not registered"
        ).toUtf8().constData() 
      )
    );
  
  BaseCommand *cmd = s_cmdRegistry.m_registeredCmds[cmdName]->createCommand();

  cmd->registrationCallBack(
    cmdName,
    s_cmdRegistry.m_registeredCmds[cmdName]->m_userData);
          
  return cmd;
}

void CommandRegistry::commandIsRegistered(const QString &cmdName) {
  std::cout << " CommandRegistry::commandIsRegistered " << cmdName.toUtf8().constData() << std::endl;
}
 