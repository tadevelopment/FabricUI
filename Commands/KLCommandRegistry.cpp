//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommand.h"
#include "KLCommandRegistry.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

KLCommandRegistry::KLCommandRegistry() 
  : CommandRegistry()
{
  COMMAND_KL = "KL";

  try 
  {
    m_klCmdRegistry = RTVal::Create(
      FabricApplicationStates::GetAppStates()->getContext(), 
      "CommandRegistry", 
      0, 0);

    m_klCmdRegistry = m_klCmdRegistry.callMethod(
      "CommandRegistry", 
      "getCommandRegistry", 
      0, 0);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLCommandManager_Python::KLCommandRegistry",
      "",
      e.getDesc_cstr());
  }
}

KLCommandRegistry::~KLCommandRegistry() 
{
}

Command* KLCommandRegistry::createCommand(
  const QString &cmdName) 
{  
  if(!isCommandRegistered(cmdName))
    FabricException::Throw( 
      "KLCommandRegistry::createCommand",
      "Cannot create command '" + cmdName + "', it's not registered"
      );

  QList<QString> spec = getCommandSpecs(
    cmdName);

  return spec[1] == COMMAND_KL
    ? createKLCommand(cmdName)
    : CommandRegistry::createCommand(cmdName);
}

FabricCore::RTVal KLCommandRegistry::getKLRegistry()
{
  return m_klCmdRegistry;
}
 
void KLCommandRegistry::synchronizeKL() 
{
  try 
  {
    RTVal klCmdNameList = m_klCmdRegistry.callMethod(
      "String[]", 
      "getRegisteredCommandList", 
      0, 0);
    
    for(unsigned int i=0; i<klCmdNameList.getArraySize(); ++i)
    {
      QString cmdName(klCmdNameList.getArrayElement(i).getStringCString());
      if(!isCommandRegistered(cmdName))
        registerKLCommand(cmdName);
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw( 
      "KLCommandRegistry::synchronizeKL",
      "",
      e.getDesc_cstr()
      );
  }
}
 
void KLCommandRegistry::registerKLCommand(
  const QString &cmdName) 
{
  try 
  {
    // Ne sure the command is registered in KL. 
    RTVal args[2] = {
      RTVal::ConstructString(
        FabricApplicationStates::GetAppStates()->getContext(), 
        cmdName.toUtf8().constData()),

      RTVal::Construct(
        FabricApplicationStates::GetAppStates()->getContext(), 
        "Type", 
        0, 0),
    };

    bool isCmdRegistered = m_klCmdRegistry.callMethod(
      "Boolean", 
      "isCommandRegistered", 
      2, 
      args).getBoolean();

  
    if(isCmdRegistered)
    {
      // The command is already registered in KL.
      // Don't call KLCommandRegistry::commandIsRegistered.
      CommandRegistry::commandIsRegistered(
        cmdName,
        RTVal::Construct(
          FabricApplicationStates::GetAppStates()->getContext(), 
          "String", 
          1, 
          &args[1]).getStringCString(),
        COMMAND_KL);
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw( 
      "KLCommandRegistry::registerKLCommand",
      "",
      e.getDesc_cstr()
      );
  }
}
 
Command* KLCommandRegistry::createKLCommand(
  const QString &cmdName)
{  
  try
  {
    RTVal args[2] = {
      RTVal::ConstructString(
        FabricApplicationStates::GetAppStates()->getContext(), 
        cmdName.toUtf8().constData()),

      RTVal::ConstructString(
        FabricApplicationStates::GetAppStates()->getContext(), 
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
    FabricException::Throw( 
      "KLCommandRegistry::createKLCommand",
      "",
      e.getDesc_cstr()
      );
  }

  return 0;
}

void KLCommandRegistry::commandIsRegistered(
  const QString &cmdName,
  const QString &cmdType,
  const QString &implType) 
{
  try
  {
    RTVal nameVal = RTVal::ConstructString(
      FabricApplicationStates::GetAppStates()->getContext(),
      cmdName.toUtf8().constData());

    m_klCmdRegistry.callMethod(
      "",
      "registerAppCommand",
      1,
      &nameVal);
  }

  catch(Exception &e)
  {
    FabricException::Throw( 
      "KLCommandRegistry::commandIsRegistered",
      "",
      e.getDesc_cstr()
      );
  }
  
  CommandRegistry::commandIsRegistered(
    cmdName,
    cmdType,
    implType);
}
