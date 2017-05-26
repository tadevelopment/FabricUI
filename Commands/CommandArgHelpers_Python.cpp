//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandArgFlags.h"
#include "BaseScriptableCommand.h"
#include "CommandArgHelpers_Python.h"
#include "BaseRTValScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

CommandArgHelpers_Python::CommandArgHelpers_Python()
{
}

inline BaseScriptableCommand* _CastToBaseScriptableCommand(
  BaseCommand *cmd) 
{
  BaseScriptableCommand* scriptCmd = qobject_cast<BaseScriptableCommand*>(cmd);

  if(scriptCmd == 0)
    FabricException::Throw(
      "CommandArgHelpers_Python::_CastToBaseScriptableCommand",
      "Bad cast");

  return scriptCmd;
}

inline BaseRTValScriptableCommand* _CastToBaseRTValScriptableCommand(
  BaseCommand *cmd) 
{
  BaseRTValScriptableCommand* scriptCmd = qobject_cast<BaseRTValScriptableCommand*>(cmd);

  if(scriptCmd == 0)
    FabricException::Throw(
      "CommandArgHelpers_Python::_CastToBaseRTValScriptableCommand",
      "Bad cast");

  return scriptCmd;
}

bool CommandArgHelpers_Python::_IsScriptableCommand_Python(
  BaseCommand *cmd)
{
  BaseScriptableCommand* scriptCmd = qobject_cast<BaseScriptableCommand*>(cmd);
  return scriptCmd != 0;
}

bool CommandArgHelpers_Python::_IsPathValueCommandArg_Python(
  const QString &key,
  BaseCommand *cmd)
{
  bool res = false;

  try 
  {
    BaseScriptableCommand* scriptCmd = _CastToBaseScriptableCommand(cmd);
    res = scriptCmd->isArg(key, CommandArgFlags::IO_ARG) ||
        scriptCmd->isArg(key, CommandArgFlags::IN_ARG) ||
      scriptCmd->isArg(key, CommandArgFlags::OUT_ARG);
  }  

  catch(FabricException &e) 
  {
    /*error = */FabricException::Throw(
      "CommandArgHelpers_Python::_IsPathValueCommandArg_Python",
      "",
      e.what(),
      PRINT
      );
  }

  return res;
}

bool CommandArgHelpers_Python::_IsCommandArg_Python(
  const QString &key,
  int flags,
  BaseCommand *cmd)
{
  bool res = false;

  try 
  {
    BaseScriptableCommand* scriptCmd = _CastToBaseScriptableCommand(cmd);
    res = scriptCmd->isArg(key, flags);
  }  

  catch(FabricException &e) 
  {
    /*error = */FabricException::Throw(
      "CommandArgHelpers_Python::_IsCommandArg_Python",
      "",
      e.what(),
      PRINT
      );
  }

  return res;
}

bool CommandArgHelpers_Python::_IsCommandArgSet_Python(
  const QString &key,
  BaseCommand *cmd)
{
  bool res = false;

  try 
  {
    BaseScriptableCommand* scriptCmd = qobject_cast<BaseScriptableCommand*>(cmd);
    res = scriptCmd->isArgSet(key);
  }  

  catch(FabricException &e) 
  {
    /*error = */FabricException::Throw(
      "CommandArgHelpers_Python::_IsCommandArgSet_Python",
      "",
      e.what(),
      PRINT
      );
  }

  return res;
}

QStringList CommandArgHelpers_Python::_GetCommandArgKeys_Python(
  BaseCommand *cmd)
{
  QStringList keys;

  try 
  {
    BaseScriptableCommand* scriptCmd = _CastToBaseScriptableCommand(cmd);
    keys = scriptCmd->getArgKeys();
  }  

  catch(FabricException &e) 
  {
    /*error = */FabricException::Throw(
      "CommandArgHelpers_Python::_GetCommandArgKeys_Python",
      "",
      e.what(),
      PRINT
      );
  }

  return keys;
}

bool CommandArgHelpers_Python::_HasCommandArg_Python(
  const QString &key,
  BaseCommand *cmd)
{
  bool res = false;

  try 
  {
    BaseScriptableCommand* scriptCmd = _CastToBaseScriptableCommand(cmd);
    res = scriptCmd->hasArg(key);
  }  

  catch(FabricException &e) 
  {
    /*error = */FabricException::Throw(
      "CommandArgHelpers_Python::_HasCommandArg_Python",
      "",
      e.what(),
      PRINT
      );
  }

  return res;
}

QString CommandArgHelpers_Python::_GetCommandArg_Python(
  const QString &key,
  BaseCommand *cmd)
{
  QString res;

  try 
  {
    BaseScriptableCommand* scriptCmd = _CastToBaseScriptableCommand(cmd);
    res = scriptCmd->getArg(key);
  }  

  catch(FabricException &e) 
  {
    /*error = */FabricException::Throw(
      "CommandArgHelpers_Python::_GetCommandArg_Python",
      "",
      e.what(),
      PRINT
      );
  }

  return res;
}

bool CommandArgHelpers_Python::_IsRTValScriptableCommand_Python(
  BaseCommand *cmd)
{
  BaseRTValScriptableCommand* scriptCmd = qobject_cast<BaseRTValScriptableCommand*>(cmd);
  return scriptCmd != 0;
}

RTVal CommandArgHelpers_Python::_GetRTValCommandArg_Python(
  const QString &key,
  BaseCommand *cmd)
{
  RTVal res;

  try 
  {
    BaseRTValScriptableCommand* scriptCmd = _CastToBaseRTValScriptableCommand(cmd);
    res = scriptCmd->getRTValArg(key);
  }  

  catch(FabricException &e) 
  {
    /*error = */FabricException::Throw(
      "CommandArgHelpers_Python::_GetRTValCommandArg_Python",
      "",
      e.what(),
      PRINT
      );
  }

  return res;
}

RTVal CommandArgHelpers_Python::_GetRTValCommandArg_Python(
  const QString &key,
  const QString &type,
  BaseCommand *cmd)
{
  RTVal res;

  try 
  {
    BaseRTValScriptableCommand* scriptCmd = _CastToBaseRTValScriptableCommand(cmd);
    res = scriptCmd->getRTValArg(key, type);
  }  

  catch(FabricException &e) 
  {
    /*error = */FabricException::Throw(
      "CommandArgHelpers_Python::_GetRTValCommandArg_Python",
      "",
      e.what(),
      PRINT
      );
  }

  return res;
}

QString CommandArgHelpers_Python::_GetRTValCommandArgType_Python(
  const QString &key,
  BaseCommand *cmd)
{
  QString res;

  try
  {
    BaseRTValScriptableCommand* scriptCmd = _CastToBaseRTValScriptableCommand(cmd);
    res = scriptCmd->getRTValArgType(key);
  }  

  catch(FabricException &e) 
  {
    /*error = */FabricException::Throw(
      "CommandArgHelpers_Python::_GetRTValCommandArgType_Python",
      "",
      e.what(),
      PRINT
      );
  }

  return res;
}
