//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include <typeinfo>
#include "KLCommand.h"
#include "CommandFactory.h"
#include "CommandRegistry.h"
#include "KLScriptableCommand.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

QString CommandRegistry::COMMAND_KL = "KL";
QString CommandRegistry::COMMAND_CPP = "CPP";
QString CommandRegistry::COMMAND_PYTHON = "PYTHON";
bool CommandRegistry::s_instanceFlag = false;
CommandRegistry* CommandRegistry::s_cmdRegistry = 0;

CommandRegistry::CommandRegistry(
  Client client) 
  : QObject()
  , m_client(client)
{
  if(s_instanceFlag)
    throw("CommandRegistry::CommandRegistry, singleton has already been created");
  
  try 
  {
    m_klCmdRegistry = RTVal::Create(
      m_client, 
      "CommandRegistry", 
      0, 
      0);

    m_klCmdRegistry = m_klCmdRegistry.callMethod(
      "CommandRegistry", 
      "getCommandRegistry", 
      0, 
      0);
  }

  catch(Exception &e)
  {
    printf(
      "CommandRegistry::CommandRegistry: exception: %s\n", 
      e.getDesc_cstr());
  }
   
  // Set the pointer of the CommandRegistry singleton
  // equal to this instance of CommandRegistry.
  s_cmdRegistry = this;
  s_instanceFlag = true;
}

CommandRegistry::~CommandRegistry() 
{
  QMapIterator<QString, BaseCommandFactory*> ite(m_registeredCmdFactories);
  while (ite.hasNext()) 
  {
    ite.next();
    BaseCommandFactory* factory = (BaseCommandFactory*)ite.value();
    delete factory;
    factory = 0;
  }
 
  s_instanceFlag = false;
}

CommandRegistry* CommandRegistry::GetCommandRegistry()
{
  if(!s_instanceFlag)
    throw(
      std::string("CommandRegistry::CommandRegistry, the registry is null")
    );

  return s_cmdRegistry;
}

bool CommandRegistry::IsInitalized()
{
  return s_instanceFlag;
}

void CommandRegistry::registerFactory(
  const QString &cmdName, 
  BaseCommandFactory *factory) 
{
  if (!isCommandRegistered(cmdName))
  {
    m_registeredCmdFactories[cmdName] = factory;

    commandRegistered(
      cmdName,
      typeid(*factory).name(),
      COMMAND_CPP);
  }
}

bool CommandRegistry::isCommandRegistered(
  const QString &cmdName) 
{
  // Check first if the command is registered
  // in KL but not in C++ alredy.
  registerKLCommand(cmdName);
  return m_registeredCmdSpecs.count(cmdName) > 0;
}

QList<QString> CommandRegistry::getCommandSpecs(
  const QString &cmdName) 
{
  QList<QString> specs;

  if(isCommandRegistered(cmdName))
    specs = m_registeredCmdSpecs[cmdName];

  return specs;
}

BaseCommand* CommandRegistry::createCommand(
  const QString &cmdName) 
{  
  if (!isCommandRegistered(cmdName))
    throw( 
      std::string(
        QString(
          "CommandRegistry::createCommand, cannot create command, '" + 
          cmdName + "' is not registered"
        ).toUtf8().constData() 
      )
    );

  QList<QString> spec = getCommandSpecs(cmdName);

  BaseCommand *cmd = 0;
  if(spec[1] == COMMAND_CPP) 
  {
    cmd = m_registeredCmdFactories[cmdName]->createCommand();

    cmd->registrationCallback(
      cmdName,
      m_registeredCmdFactories[cmdName]->m_userData);
  }

  else if(spec[1] == COMMAND_KL)
    cmd = createKLCommand(cmdName);
  
  return cmd;
}

FabricCore::RTVal CommandRegistry::getKLRegistry()
{
  return m_klCmdRegistry;
}

FabricCore::Client CommandRegistry::getFabricClient()
{
  return m_client;
}

void CommandRegistry::synchronizeKL() 
{
  try 
  {
    RTVal klCmdNameList = m_klCmdRegistry.callMethod(
      "String[]", 
      "getRegisteredCommandList", 
      0, 
      0);
    
    for(unsigned int i=0; i<klCmdNameList.getArraySize(); ++i)
    {
      QString cmdName(klCmdNameList.getArrayElement(i).getStringCString());
      registerKLCommand(cmdName);
    }
  }

  catch(Exception &e)
  {
    printf(
      "CommandRegistry::synchronizeKL: exception: %s\n", 
      e.getDesc_cstr());
  }
}

QString CommandRegistry::getContent()
{
  QString res = "--> CommandRegistry:\n";
  QMapIterator< QString, QList<QString> > specsIt(m_registeredCmdSpecs);

  while (specsIt.hasNext()) 
  {
    specsIt.next();
    QString name = specsIt.key();
    QList<QString> spec = specsIt.value();

    res += QString(
      "["+ name + "] type:" + spec[0] + ", implType:" + spec[1] + "\n"
    );
  }

  return res;
}

void CommandRegistry::registerKLCommand(
  const QString &cmdName) 
{
  try 
  {
    RTVal args[2] = {
      RTVal::ConstructString(m_client, cmdName.toUtf8().constData()),
      RTVal::Construct(m_client, "Type", 0, 0),
    };

    bool isCmdRegistered = m_klCmdRegistry.callMethod(
      "Boolean", 
      "isCommandRegistered", 
      2, 
      args).getBoolean();

    if(isCmdRegistered)
    {
      commandRegistered(
        cmdName,
        RTVal::Construct(
          m_client, 
          "String", 
          1, 
          &args[1]).getStringCString(),
        COMMAND_KL);
    }
  }

  catch(Exception &e)
  {
    printf(
      "CommandRegistry::registerKLCommand: exception: %s\n", 
      e.getDesc_cstr());
  }
}

BaseCommand* CommandRegistry::createKLCommand(
  const QString &cmdName)
{  
  try
  {
    RTVal args[2] = {
      RTVal::ConstructString(
        m_client, 
        cmdName.toUtf8().constData()),

      RTVal::ConstructString(
        m_client, 
        "")
    };

    // Creates the KL command from the KL registery. 
    // Check if it's a scriptable command
    RTVal klCmd = m_klCmdRegistry.callMethod(
      "BaseScriptableCommand", 
      "createCommand", 
      2, 
      args);

    if(!klCmd.isNullObject())
      return new KLScriptableCommand(klCmd);
    
    // if not, it's a simple command.
    else
    {
      klCmd = m_klCmdRegistry.callMethod(
        "Command", 
        "createCommand", 
        2, 
        args);

      return new KLCommand(klCmd);
    }
  }

  catch(Exception &e)
  {
    printf(
      "CommandRegistry::createKLCommand: exception: %s\n", 
      e.getDesc_cstr());
  }

  return 0;
}

void CommandRegistry::commandRegistered(
  const QString &cmdName,
  const QString &cmdType,
  const QString &implType) 
{
  if(m_registeredCmdSpecs.count(cmdName))
    return;

  // sets the command specs
  QList<QString> spec;
  spec.append(cmdType);
  spec.append(implType);
  m_registeredCmdSpecs[cmdName] = spec;

  // Inform the KL registry
  try
  {
    RTVal nameVal = RTVal::ConstructString(
      m_client,
      cmdName.toUtf8().constData());

    m_klCmdRegistry.callMethod(
      "",
      "registerAppCommand",
      1,
      &nameVal);
  }

  catch(Exception &e)
  {
    printf(
      "CommandRegistry::commandRegistered: exception: %s\n", 
      e.getDesc_cstr());
  }
   
  // inform a command has been registered.
  emit commandRegisteredCallback(
    cmdName,
    cmdType,
    implType);
}
