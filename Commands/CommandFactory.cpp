//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include "CommandFactory.h"

using namespace FabricUI;
using namespace Commands;


QMap<QString, BaseCommandFactory*> CommandFactoryRegistry::s_registeredCmds;
 
CommandFactoryRegistry::CommandFactoryRegistry() 
{
}

void CommandFactoryRegistry::RegisterFactory(
  QString cmdName, 
  BaseCommandFactory *factory) 
{
  if (!IsCommandRegistered(cmdName))
    s_registeredCmds[cmdName] = factory;
}

bool CommandFactoryRegistry::IsCommandRegistered(
  QString cmdName) 
{
  return s_registeredCmds.count(cmdName) != 0;
}

BaseCommand* CommandFactoryRegistry::CreateCommand(
  QString cmdName) 
{
  if (!IsCommandRegistered(cmdName))
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
