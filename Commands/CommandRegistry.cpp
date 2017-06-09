//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandRegistry.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace Application;

bool CommandRegistry::s_instanceFlag = false;
CommandRegistry* CommandRegistry::s_cmdRegistry = 0;

CommandRegistry::CommandRegistry() 
  : Util::BaseFactoryRegistry()
{
  if(s_instanceFlag)
    FabricException::Throw(
      "CommandRegistry::CommandRegistry",
      "singleton has already been created");
   
  COMMAND_CPP = "CPP";
 
  // Set the pointer of the CommandRegistry singleton
  // equal to this instance of CommandRegistry.
  s_cmdRegistry = this;
  s_instanceFlag = true;
}

CommandRegistry::~CommandRegistry() 
{
  s_instanceFlag = false;
}

CommandRegistry* CommandRegistry::getCommandRegistry()
{
  if(!s_instanceFlag)
    FabricException::Throw(
      "CommandRegistry::getCommandRegistry",
      "the registry is null");

  return s_cmdRegistry;
}

bool CommandRegistry::isInitalized()
{
  return s_instanceFlag;
}

void CommandRegistry::registerFactory(
  const QString &name, 
  Factory *factory) 
{
  if(!isCommandRegistered(name))
  {
    Util::BaseFactoryRegistry::registerFactory(
      name, 
      factory);
    
    commandIsRegistered(
      name,
      // Get the name of the cmd class.
      factory->getType(),
      COMMAND_CPP);
  }
}

void CommandRegistry::unregisterFactory(
  const QString &name)
{
  // Does nothing.
}

bool CommandRegistry::isCommandRegistered(
  const QString &cmdName) 
{
  return m_cmdSpecs.count(cmdName) > 0;
}

QPair<QString, QString> CommandRegistry::getCommandSpecs(
  const QString &cmdName) 
{
  if(!isCommandRegistered(cmdName))
    FabricException::Throw( 
      "CommandRegistry::getCommandSpecs",
      "cannot create command '" + cmdName + "', it's not registered");

  return m_cmdSpecs[cmdName];
}

QList<QString> CommandRegistry::getCommandNames() 
{
  return m_cmdSpecs.keys();
}

BaseCommand* CommandRegistry::createCommand(
  const QString &cmdName) 
{  
  if(!isCommandRegistered(cmdName))
    FabricException::Throw( 
      "CommandRegistry::createCommand",
      "cannot create command '" + cmdName + "', it's not registered");

  FABRIC_CATCH_BEGIN();

  QPair<QString, QString> spec = getCommandSpecs(cmdName);
  
  if(spec.second == COMMAND_CPP) 
  {
    Factory *factory = Util::BaseFactoryRegistry::getFactory(
      cmdName);

    BaseCommand* cmd = (BaseCommand*)factory->create(); 
    if(cmd == 0)
      FabricException::Throw(
        "CommandRegistry::createCommand",
        "resulting command is null");

    void *userData = factory->getUserData();
  
    cmd->registrationCallback(
      cmdName,
      userData);

    return cmd;
  }

  FABRIC_CATCH_END("CommandRegistry::createCommand");

  return 0;
}

QString CommandRegistry::getContent()
{
  QString res = "--> CommandRegistry:\n";
  QMapIterator< QString, QPair<QString, QString> > specsIt(m_cmdSpecs);

  while(specsIt.hasNext()) 
  {
    specsIt.next();
    QString name = specsIt.key();
    QPair<QString, QString> spec = specsIt.value();

    res += QString(
      "["+ name + "] type:" + spec.first + ", implType:" + spec.second + "\n"
    );
  }

  return res;
}

void CommandRegistry::commandIsRegistered(
  const QString &cmdName,
  const QString &cmdType,
  const QString &implType) 
{
  // sets the command specs
  QPair<QString, QString> spec;
  spec.first = cmdType;
  spec.second = implType;
  m_cmdSpecs[cmdName] = spec;
    
  // inform a command has been registered.
  emit commandRegistered(
    cmdName,
    cmdType,
    implType);
}
