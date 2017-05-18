//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandException.h"
#include "KLCommandHelpers.h"
#include "KLCommandRegistry.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Util/RTValUtil.h>
 
using namespace FabricUI;
using namespace Util;
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

bool KLScriptableCommand::addToUndoStack() 
{
  return AddKLCommandToUndoStack(m_klCmd);
}

bool KLScriptableCommand::canLog() 
{
  return CanKLCommandLog(m_klCmd);
}

bool KLScriptableCommand::doIt() 
{ 
  RTVal cmd = RTVal::Construct(
    m_klCmd.getContext(),
    "Command",
    1,
    &m_klCmd);
  
  return DoKLCommand(cmd);
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
  return GetKLCommandHelp(
    m_klCmd);
}

QString KLScriptableCommand::getHistoryDesc() 
{
  return GetKLCommandHistoryDesc(
    m_klCmd);
}

// ScriptableCommand
void KLScriptableCommand::declareArg( 
  const QString &key, 
  int flag, 
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
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry*>(
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
    CommandException::Throw(
      "KLScriptableCommand::hasArg", 
      "",
      e.getDesc_cstr());
  }

  return res;
}

bool KLScriptableCommand::isArg(
  const QString &key,
  int flag) 
{
  bool res = false;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry*>(
      Commands::CommandRegistry::GetCommandRegistry());

    RTVal args[2] = 
    {
      RTVal::ConstructString(
        registry->getClient(), 
        key.toUtf8().constData()),

      RTVal::ConstructUInt32(
        registry->getClient(), 
        flag)
    };
 
    res = m_klCmd.callMethod(
      "Boolean",
      "isArg",
      2,
      args
      ).getBoolean();
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "KLScriptableCommand::isArg", 
      "",
      e.getDesc_cstr());
  }

  return res;
}

QList<QString> KLScriptableCommand::getArgKeys()
{
  QList<QString> keys;

  try 
  {
    RTVal args = m_klCmd.callMethod(
      "RTVal[String]", 
      "getArgs", 
      0, 0);

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
    CommandException::Throw(
      "KLScriptableCommand::getArgKeys", 
      "",
      e.getDesc_cstr());
  }
  
  return keys;
}

bool KLScriptableCommand::isArgSet(
  const QString &key)
{
  return true;
}

QString KLScriptableCommand::getArg(
  const QString &key) 
{
  return RTValUtil::forceRTValToJSON(
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
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry*>(
      Commands::CommandRegistry::GetCommandRegistry());

    // Get the argument from its JSON description.
    RTVal klRTVal = RTValUtil::forceJSONToRTVal(
        registry->getClient(),
        json,
        getRTValArgType(key));

    setRTValArg(key, klRTVal);
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "KLScriptableCommand::setArg",
      e.getDesc_cstr(),
      "");
  }
}

void KLScriptableCommand::validateSetArgs()
{
  QString strError;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry*>(
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
    CommandException::Throw(
      "KLScriptableCommand::validateSetArgs", 
      "",
      e.getDesc_cstr());
  }
  
  if(!strError.isEmpty())
    CommandException::Throw(
      "KLScriptableCommand::validateSetArgs",
      "",
      strError);
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
    CommandException::Throw(
      "KLScriptableCommand::getArgsDescription", 
      "",
      e.getDesc_cstr());
  }

  return "";
}

// RTValScriptableCommand
void KLScriptableCommand::declareRTValArg( 
  const QString &key, 
  const QString &type,
  int flag, 
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
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry*>(
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
    CommandException::Throw(
      "KLScriptableCommand::getRTValArgType", 
      "",
      e.getDesc_cstr());
  }
  
  return res;
}

void KLScriptableCommand::setRTValArgType(
  const QString &key,
  const QString &type)
{
  // Do nothing
}

RTVal KLScriptableCommand::getRTValArg(
  const QString &key)
{
  if(!hasArg(key)) 
    CommandException::Throw(
      "KLScriptableCommand::getRTValArgType",
      "No arg named '" + key + "' in command '" + getName() + "'",
      "");

  RTVal res;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry*>(
      Commands::CommandRegistry::GetCommandRegistry());

    RTVal keyVal = RTVal::ConstructString(
      registry->getClient(), 
      key.toUtf8().constData());

    res = RTValUtil::forceToRTVal(m_klCmd.callMethod(
      "RTVal", 
      "getArg", 
      1, 
      &keyVal));
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "KLScriptableCommand::getRTValArg",
      "",
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
  QString strError;

  try 
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry*>(
      Commands::CommandRegistry::GetCommandRegistry());
    
    RTVal keyVal = RTVal::ConstructString(
      registry->getClient(), 
      key.toUtf8().constData());
    
    RTVal args[3] = { 
      keyVal, 
      RTValUtil::forceToKLRTVal(value), 
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
    if(!strError.isEmpty())
      CommandException::Throw(
        "KLScriptableCommand::setRTValArg",
        "",
        strError);
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "KLScriptableCommand::setRTValArg",
      strError,
      e.getDesc_cstr());
  }
}
