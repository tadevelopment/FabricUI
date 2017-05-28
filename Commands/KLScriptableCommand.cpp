//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommandHelpers.h"
#include "KLCommandRegistry.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>
 
using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

KLScriptableCommand::KLScriptableCommand(
  RTVal klCmd)
  : BaseRTValScriptableCommand()
  , m_klCmd(klCmd)
{
}

KLScriptableCommand::~KLScriptableCommand() 
{
}

// BaseCommand
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
    "BaseCommand",
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

// BaseScriptableCommand
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
    RTVal keyVal = RTVal::ConstructString(
      m_klCmd.getContext(), 
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
    FabricException::Throw(
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
    RTVal args[2] = 
    {
      RTVal::ConstructString(
        m_klCmd.getContext(), 
        key.toUtf8().constData()),

      RTVal::ConstructUInt32(
        m_klCmd.getContext(), 
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
    FabricException::Throw(
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
    FabricException::Throw(
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
  return RTValUtil::toJSON(
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
    // Get the argument from its JSON description.
    RTVal klRTVal = RTValUtil::fromJSON(
        m_klCmd.getContext(),
        json,
        getRTValArgType(key));

    setRTValArg(key, klRTVal);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
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
    RTVal errorVal = RTVal::ConstructString(
      m_klCmd.getContext(), 
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
    FabricException::Throw(
      "KLScriptableCommand::validateSetArgs", 
      "",
      e.getDesc_cstr());
  }
  
  if(!strError.isEmpty())
    FabricException::Throw(
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
    FabricException::Throw(
      "KLScriptableCommand::getArgsDescription", 
      "",
      e.getDesc_cstr());
  }

  return "";
}

// BaseRTValScriptableCommand
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
    RTVal keyVal = RTVal::ConstructString(
      m_klCmd.getContext(), 
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
    FabricException::Throw(
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
    FabricException::Throw(
      "KLScriptableCommand::getRTValArgType",
      "No arg named '" + key + "' in command '" + getName() + "'",
      "");

  RTVal res;

  try 
  {
    RTVal keyVal = RTVal::ConstructString(
      m_klCmd.getContext(), 
      key.toUtf8().constData());

    res = RTValUtil::toRTVal(m_klCmd.callMethod(
      "RTVal", 
      "getArg", 
      1, 
      &keyVal));
  }

  catch(Exception &e)
  {
    FabricException::Throw(
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
    RTVal keyVal = RTVal::ConstructString(
      m_klCmd.getContext(), 
      key.toUtf8().constData());
    
    RTVal args[3] = { 
      keyVal, 
      RTValUtil::toKLRTVal(value), 
      // error
      RTVal::ConstructString(
        m_klCmd.getContext(), 
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
      FabricException::Throw(
        "KLScriptableCommand::setRTValArg",
        "",
        strError);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLScriptableCommand::setRTValArg",
      strError,
      e.getDesc_cstr());
  }
}
