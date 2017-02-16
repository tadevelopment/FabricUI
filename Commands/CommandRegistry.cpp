//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include <typeinfo>
#include "CommandFactory.h"
#include "CommandRegistry.h"

using namespace FabricUI;
using namespace Commands;

bool CommandRegistry::s_instanceFlag = false;
CommandRegistry* CommandRegistry::s_cmdRegistry = 0;

CommandRegistry::CommandRegistry() 
{
}

CommandRegistry::~CommandRegistry() 
{
  s_instanceFlag = false;
}

void CommandRegistry::SetCommandRegistrySingleton(
  CommandRegistry* registery)
{
  if(!s_instanceFlag)
  {
    s_cmdRegistry = registery;
    s_instanceFlag = true;
  }
}

CommandRegistry* CommandRegistry::GetCommandRegistry()
{
  if(!s_instanceFlag)
  {
    s_cmdRegistry = new CommandRegistry();
    s_instanceFlag = true;
  }
  
  return s_cmdRegistry;
}

void CommandRegistry::registerFactory(
  const QString &cmdName, 
  BaseCommandFactory *factory) 
{
  QString factoryType;
  if (!isCommandRegistered(cmdName, factoryType))
  {

    m_registeredCmds[cmdName] = factory;
    commandIsRegistered(
      cmdName,
      QString(typeid(*factory).name()));
  }
}

bool CommandRegistry::isCommandRegistered(
  const QString &cmdName,
  QString &factoryType) 
{
  bool isRegistered = m_registeredCmds.count(cmdName) != 0;
  factoryType = isRegistered ? typeid(*m_registeredCmds[cmdName]).name() : "";
  return isRegistered;
}

BaseCommand* CommandRegistry::createCommand(
  const QString &cmdName) 
{
  QString factoryType;
  if (!isCommandRegistered(cmdName, factoryType))
    throw( 
      std::string(
        QString(
          "Cannot create command, '" + cmdName + "' is not registered"
        ).toUtf8().constData() 
      )
    );
  
  BaseCommand *cmd = m_registeredCmds[cmdName]->createCommand();

  cmd->registrationCallBack(
    cmdName,
    m_registeredCmds[cmdName]->m_userData);
          
  return cmd;
}

void CommandRegistry::commandIsRegistered(
  const QString &cmdName,
  const QString &cmdType) 
{
}
