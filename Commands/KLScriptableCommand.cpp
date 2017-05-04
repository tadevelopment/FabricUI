//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandHelpers.h"
#include "KLCommandHelpers.h"
#include "KLCommandRegistry.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

KLScriptableCommand::KLScriptableCommand(
  RTVal klCmd)
  : BaseRTValScriptableCommand()
  , m_klCmd(klCmd)
{
}

KLScriptableCommand::~KLScriptableCommand() 
{
}

// Command
QString KLScriptableCommand::getName() 
{
  return GetKLCommandName(m_klCmd);
}

bool KLScriptableCommand::canUndo() 
{
  return CanKLCommandUndo(m_klCmd);
}

bool KLScriptableCommand::doIt() 
{ 
  return DoKLCommand(
    m_klCmd.callMethod(
      "Command", 
      "getAsCommand", 
      0, 
      0));
}

bool KLScriptableCommand::undoIt() 
{ 
  return UndoKLCommand();
}

bool KLScriptableCommand::redoIt() 
{  
  return RedoKLCommand();
}

QString KLScriptableCommand::getHelp() 
{
  return GetKLCommandHelp(m_klCmd);
}

// ScriptableCommand
void KLScriptableCommand::declareArg( 
  const QString &key, 
  bool optional, 
  const QString &defaultValue)
{
  // Do nothing.
}

bool KLScriptableCommand::hasArg(
  const QString &key) 
{
  bool res = false;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
      Commands::CommandRegistry::GetCommandRegistry());

    RTVal keyVal = RTVal::ConstructString(
      registry->getClient(), 
      key.toUtf8().constData());

    res = m_klCmd.callMethod(
      "Boolean",
      "hasArg",
      1,
      &keyVal
      ).getBoolean();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::hasArg: exception: %s\n", 
      e.getDesc_cstr());
  }

  return res;
}

QString KLScriptableCommand::getArg(
  const QString &key) 
{
  return Util::RTValUtil::klRTValToJSON(
    getRTValArg(
      key)
    );
}
 
void KLScriptableCommand::setArg(
  const QString &key, 
  const QString &json) 
{
  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
      Commands::CommandRegistry::GetCommandRegistry());

    // Get the argument from its JSON description.
    RTVal klRTVal = Util::RTValUtil::jsonToKLRTVal(
      registry->getClient(),
      json,
      getRTValArgType(key));

    setRTValArg(key, klRTVal);
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::setArg: exception: %s\n", 
      e.getDesc_cstr());
  }
}

QList<QString> KLScriptableCommand::getArgKeys()
{
  QList<QString> keys;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
      Commands::CommandRegistry::GetCommandRegistry());

    RTVal errorVal = RTVal::ConstructString(
      registry->getClient(), 
      "");

    RTVal args = m_klCmd.callMethod(
      "", 
      "getArgs", 
      0, 
      0);

    RTVal rtvalKeys = args.getDictKeys();
    for (unsigned i = 0; i < rtvalKeys.getArraySize(); i++) 
    {
      QString key = rtvalKeys.getArrayElementRef(
        i).getStringCString(); 

      keys.append(key);
    }
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::getArgKeys: exception: %s\n", 
      e.getDesc_cstr());
  }
  
  return keys;
}

void KLScriptableCommand::validateSetArgs()
{
  QString strError;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
      Commands::CommandRegistry::GetCommandRegistry());

    RTVal errorVal = RTVal::ConstructString(
      registry->getClient(), 
      "");

    m_klCmd.callMethod(
      "", 
      "validateSetArgs", 
      1, 
      &errorVal);

    strError = errorVal.getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::validateSetArgs: exception: %s\n", 
      e.getDesc_cstr());
  }
  
  if(!strError.isEmpty())
     printAndThrow(
      strError.toUtf8().constData()
    );
}

QString KLScriptableCommand::getArgsDescription() 
{ 
  try 
  {
    return m_klCmd.callMethod(
      "String",
      "getArgsDescription", 
      0, 
      0).getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::getArgsDescription: exception: %s\n", 
      e.getDesc_cstr());
  }

  return "";
}

// RTValScriptableCommand
void KLScriptableCommand::declareRTValArg( 
  const QString &key, 
  const QString &type,
  bool optional, 
  const QString &defaultValue)
{
  // Do nothing.
}

void KLScriptableCommand::declareRTValArg( 
  const QString &key, 
  const QString &type,
  bool optional, 
  FabricCore::RTVal defaultValue)
{
  // Do nothing.
}

QString KLScriptableCommand::getRTValArgType(
  const QString &key)
{
  QString res;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
      Commands::CommandRegistry::GetCommandRegistry());

    RTVal keyVal = RTVal::ConstructString(
      registry->getClient(), 
      key.toUtf8().constData());

    res = m_klCmd.callMethod(
      "String", 
      "getArgType", 
      1, 
      &keyVal
      ).getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::getRTValArgType: exception: %s\n", 
      e.getDesc_cstr());
  }
  
  return res;
}

RTVal KLScriptableCommand::getRTValArg(
  const QString &key)
{
  if(!hasArg(key)) 
    printAndThrow(
      std::string(
        "KLScriptableCommand::getRTValArg, error: no arg named " +
        std::string(key.toUtf8().constData())
      )
    );

  RTVal res;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
      Commands::CommandRegistry::GetCommandRegistry());

    RTVal keyVal = RTVal::ConstructString(
      registry->getClient(), 
      key.toUtf8().constData());

    res = m_klCmd.callMethod(
      "String", 
      "getArg", 
      1, 
      &keyVal);
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::getRTValArg: exception: %s\n", 
      e.getDesc_cstr());
  }
  
  return res;
}

RTVal KLScriptableCommand::getRTValArg(
  const QString &key,
  const QString &type)
{
  return getRTValArg(key);
}

void KLScriptableCommand::setRTValArg( 
  const QString &key, 
  FabricCore::RTVal value)
{
  if(!hasArg(key)) 
    printAndThrow(
      std::string(
        "KLScriptableCommand::setRTValArg, error: no arg named " +
        std::string(key.toUtf8().constData())
      )
    );

  QString strError;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
      Commands::CommandRegistry::GetCommandRegistry());
    
    RTVal keyVal = RTVal::ConstructString(
      registry->getClient(), 
      key.toUtf8().constData());
    
    RTVal args[3] = { 
      keyVal, 
      value, 
      // error
      RTVal::ConstructString(
        registry->getClient(), 
        "") 
    };

    m_klCmd.callMethod(
      "", 
      "setArg", 
      3, 
      args);

    // Gets possible KL errors.
    strError = args[2].getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "KLScriptableCommand::setRTValArg: exception: %s\n", 
      e.getDesc_cstr());
  }

  if(!strError.isEmpty())
    printAndThrow(
      strError.toUtf8().constData()
    );
}