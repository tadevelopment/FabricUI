//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include "CommandFactory.h"
#include <typeinfo>

using namespace FabricUI;
using namespace Commands;


QMap<QString, BaseCommandFactory*> CommandFactoryRegistry::s_registeredCmds;
 
CommandFactoryRegistry::CommandFactoryRegistry() 
{
}

void CommandFactoryRegistry::RegisterFactory(
  const QString &cmdName, 
  BaseCommandFactory *factory) 
{
  QString factoryType;
  if (!IsCommandRegistered(cmdName, factoryType))
    s_registeredCmds[cmdName] = factory;
}

bool CommandFactoryRegistry::IsCommandRegistered(
  const QString &cmdName,
  QString &factoryType) 
{
  bool isRegistered = s_registeredCmds.count(cmdName) != 0;
  factoryType = isRegistered ? typeid(*s_registeredCmds[cmdName]).name() : "";
  return isRegistered;
}

BaseCommand* CommandFactoryRegistry::CreateCommand(
  const QString &cmdName) 
{
  QString factoryType;
  if (!IsCommandRegistered(cmdName, factoryType))
    throw( 
      std::string(
        QString("Cannot create command, '" + cmdName + "' is not registered").toUtf8().constData() 
      )
    );
  
  BaseCommand *cmd = s_registeredCmds[cmdName]->createCommand();

  cmd->registrationCallBack(
    cmdName,
    s_registeredCmds[cmdName]->m_userData);
          
  return cmd;
}

BaseCommandFactory::BaseCommandFactory(
  void *userData) 
  : m_userData(userData)
{
}

BaseCommandFactory::~BaseCommandFactory()
{
}

BaseCommand *BaseCommandFactory::createCommand() 
{ 
  return 0; 
}
