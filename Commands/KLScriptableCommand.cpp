//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLCommandHelpers.h"
#include "KLScriptableCommand.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

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

bool KLScriptableCommand::canLog() 
{
  return CanKLCommandLog(m_klCmd);
}

bool KLScriptableCommand::doIt() 
{ 
  FABRIC_CATCH_BEGIN();

  RTVal cmd = RTVal::Construct(
    m_klCmd.getContext(),
    "Command",
    1,
    &m_klCmd);
  
  return DoKLCommand(cmd);

  FABRIC_CATCH_END("KLScriptableCommand::doIt");

  return false;
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

QString KLScriptableCommand::getHistoryDesc() 
{
  return GetKLCommandHistoryDesc(m_klCmd);
}

void KLScriptableCommand::setCanMergeID(
  int canMergeID)
{
  SetKLCommandCanMergeID(m_klCmd, canMergeID);
}

int KLScriptableCommand::getCanMergeID()
{
  return GetKLCommandCanMergeID(m_klCmd);
}

bool KLScriptableCommand::canMerge(
  BaseCommand *cmd) 
{
  KLScriptableCommand* scriptCmd = qobject_cast<KLScriptableCommand*>(cmd);
  if(scriptCmd == 0)
    return false;
  
  return CanMergeKLCommand(m_klCmd, scriptCmd->m_klCmd);
}

void KLScriptableCommand::merge(
  BaseCommand *cmd) 
{
  KLScriptableCommand* scriptCmd = qobject_cast<KLScriptableCommand*>(cmd);
  MergeKLCommand(m_klCmd, scriptCmd->m_klCmd);
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
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return m_klCmd.callMethod(
    "Boolean",
    "hasArg",
    1,
    &keyVal
    ).getBoolean();

  FABRIC_CATCH_END("KLScriptableCommand::hasArg");

  return false;
}

bool KLScriptableCommand::isArg(
  const QString &key,
  int flag) 
{
  FABRIC_CATCH_BEGIN();

  RTVal args[2] = 
  {
    RTVal::ConstructString(
      m_klCmd.getContext(), 
      key.toUtf8().constData()),

    RTVal::ConstructUInt32(
      m_klCmd.getContext(), 
      flag)
  };

  return m_klCmd.callMethod(
    "Boolean",
    "isArg",
    2,
    args
    ).getBoolean();
  
  FABRIC_CATCH_END("KLScriptableCommand::isArg");

  return false;
}

QList<QString> KLScriptableCommand::getArgKeys()
{
  QList<QString> keys;

  FABRIC_CATCH_BEGIN();

  RTVal rtvalKeys = m_klCmd.callMethod(
    "String[]", 
    "getArgKeys", 
    0, 0);

  for (unsigned i = 0; i < rtvalKeys.getArraySize(); i++) 
    keys.append(rtvalKeys.getArrayElementRef(
      i).getStringCString()); 

  FABRIC_CATCH_END("KLScriptableCommand::getArgKeys");
  
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
  FABRIC_CATCH_BEGIN();

  return RTValUtil::toJSON(getRTValArg(key));

  FABRIC_CATCH_END("KLScriptableCommand::getArg");

  return "";
}

void KLScriptableCommand::setArg(
  const QString &key, 
  const QString &json) 
{
  checkHasArg("KLScriptableCommand::setArg", key);

  FABRIC_CATCH_BEGIN();

  if(isJSONPathValueArg(json))
    setRTValArg(
      key, 
      RTValUtil::fromJSON(m_klCmd.getContext(), json, "PathValue")
      );
 
  else
    setRTValArgValue(
      key, 
      RTValUtil::fromJSON(m_klCmd.getContext(), json, getRTValArgType(key))
      );

  FABRIC_CATCH_END("KLScriptableCommand::setArg");
}

void KLScriptableCommand::validateSetArgs()
{
  FABRIC_CATCH_BEGIN();

  RTVal errorVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    "");

  m_klCmd.callMethod(
    "", 
    "validateSetArgs", 
    1, 
    &errorVal);

  // Get possible KL error.
  QString strError = errorVal.getStringCString();
  if(!strError.isEmpty())
    FabricException::Throw(
      "KLScriptableCommand::validateSetArgs",
      "",
      strError);

  FABRIC_CATCH_END("KLScriptableCommand::validateSetArgs");
}

QString KLScriptableCommand::getArgsDescription() 
{ 
  FABRIC_CATCH_BEGIN();

  return m_klCmd.callMethod(
    "String",
    "getArgsDescription", 
    0, 
    0).getStringCString();

  FABRIC_CATCH_END("KLScriptableCommand::getArgsDescription");

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
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return m_klCmd.callMethod(
    "String", 
    "getArgType", 
    1, 
    &keyVal
    ).getStringCString();

  FABRIC_CATCH_END("KLScriptableCommand::getRTValArgType");

  return "";
}

QString KLScriptableCommand::getRTValArgPath(
  const QString &key)
{
  checkHasArg("KLScriptableCommand::getRTValArgPath", key);
 
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return m_klCmd.callMethod(
    "String", 
    "getArgValue", 
    1, 
    &keyVal
    ).getStringCString();

  FABRIC_CATCH_END("KLScriptableCommand::getRTValArgPath");

  return "";
}

RTVal KLScriptableCommand::getRTValArgValue(
  const QString &key)
{
  checkHasArg("KLScriptableCommand::getRTValArgValue", key);
 
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return RTValUtil::toRTVal(m_klCmd.callMethod(
    "RTVal", 
    "getArgValue", 
    1, 
    &keyVal));

  FABRIC_CATCH_END("KLScriptableCommand::getRTValArgPath");

  return RTVal();
}

RTVal KLScriptableCommand::getRTValArgValue(
  const QString &key,
  const QString &type)
{
  return getRTValArgValue(key);
}

void KLScriptableCommand::setRTValArgValue( 
  const QString &key, 
  FabricCore::RTVal value)
{
  FABRIC_CATCH_BEGIN();

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
    "setArgValue", 
    3, 
    args);

  // Get possible KL error.
  QString strError = args[2].getStringCString();
  if(!strError.isEmpty())
    FabricException::Throw(
      "KLScriptableCommand::setRTValArgValue",
      "",
      strError);

  FABRIC_CATCH_END("KLScriptableCommand::setRTValArgValue");
}

RTVal KLScriptableCommand::getRTValArg(
  const QString &key)
{
  checkHasArg("KLScriptableCommand::getRTValArg", key);
 
  FABRIC_CATCH_BEGIN();

  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());

  return RTValUtil::toRTVal(m_klCmd.callMethod(
    "RTVal", 
    "getArg", 
    1, 
    &keyVal));

  FABRIC_CATCH_END("KLScriptableCommand::getRTValArg");

  return RTVal();
}

void KLScriptableCommand::setRTValArg( 
  const QString &key, 
  FabricCore::RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
 
  RTVal keyVal = RTVal::ConstructString(
    m_klCmd.getContext(), 
    key.toUtf8().constData());
  
  RTVal args[3] = { 
    keyVal, 
    RTValUtil::toRTVal(pathValue), 
    // error
    RTVal::ConstructString(m_klCmd.getContext(), "") 
  };

  m_klCmd.callMethod(
    "", 
    "setArg", 
    3, 
    args);

  // Get possible KL error.
  QString strError = args[2].getStringCString();
  if(!strError.isEmpty())
    FabricException::Throw(
      "KLScriptableCommand::setRTValArg",
      "",
      strError);

  FABRIC_CATCH_END("KLScriptableCommand::setRTValArg");
}
