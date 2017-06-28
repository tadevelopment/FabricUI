//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseCommand.h"
#include "KLCommandManager.h"
#include "CommandArgHelpers.h"
#include "BaseScriptableCommand.h"
#include "BaseRTValScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>
 
using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

/// Optional argument flag
int CommandArgFlags::NO_FLAG_ARG = 0;
/// Optional argument flag
int CommandArgFlags::OPTIONAL_ARG = 1;
/// Loggale argument flag
int CommandArgFlags::DONT_LOG_ARG = 2;
/// Input argument flag
int CommandArgFlags::IN_ARG = 4;
/// Output argument flag
int CommandArgFlags::OUT_ARG = 8;
/// IO argument flag
int CommandArgFlags::IO_ARG = 16;

QString CommandArgHelpers::createHistoryDescFromArgs(
  QMap<QString, QString> const&argsDesc,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();

  BaseRTValScriptableCommand *rtValScriptCmd = qobject_cast<BaseRTValScriptableCommand*>(
    cmd);

  QString desc = cmd->getName();

  if(argsDesc.size() > 0)
  {
    int count = 0;
    desc +=  "(";

    QMapIterator<QString, QString> it(argsDesc);
    while(it.hasNext()) 
    {
       it.next();

      QString key = it.key();
      QString argDesc = ( (rtValScriptCmd != 0) && !rtValScriptCmd->getRTValArgPath(key).isEmpty())
        ? "<" + it.value() + ">"
        : it.value();

      desc += key + "=\"" + argDesc + "\"";
      if(count < argsDesc.size()-1)
        desc += ", ";
      count++;
    }

    desc +=  ")";
  }

  return desc;

  FABRIC_CATCH_END("CommandArgHelpers::createHistoryDescFromArgs");

  return "";
}

QString CommandArgHelpers::getArgsTypeSpecs(
  BaseCommand *cmd,
  QString const&key)
{
  BaseScriptableCommand *scriptCmd = qobject_cast<BaseScriptableCommand*>(
    cmd);
 
  QString specs; 

  if(scriptCmd &&scriptCmd->hasArg(key)) 
  {
    specs += "["; 

    BaseRTValScriptableCommand *rtValScriptCmd = qobject_cast<BaseRTValScriptableCommand*>(
      cmd);

    if(rtValScriptCmd)
      specs += "PathValue(" + rtValScriptCmd->getRTValArgType(key) + ")"; 
    else
      specs += "String";

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::OPTIONAL_ARG))
      specs += ", optional"; 

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::DONT_LOG_ARG ))
      specs += ", dont-log"; 

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::IN_ARG))
      specs += ", IN"; 

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::OUT_ARG))
      specs += ", OUT"; 

    if(scriptCmd->hasArgFlag(key, CommandArgFlags::IO_ARG))
      specs += ", IO"; 
    
    specs += "]"; 
  }

  return specs;
}

QString CommandArgHelpers::createHelpFromArgs(
  QString const&commandHelp,
  QMap<QString, QString> const&argsHelp,
  BaseCommand *cmd)
{
  QString help;
  
  FABRIC_CATCH_BEGIN();

  BaseScriptableCommand *scriptCmd = qobject_cast<BaseScriptableCommand*>(
    cmd);

  if(scriptCmd == 0) 
    FabricException::Throw(
      "CommandArgHelpers::createHelpFromArgs",
      "cmd '" + cmd->getName() + "' is not a BaseScriptableCommand");

  help = commandHelp + "\n";

  if(argsHelp.size() > 0)
    help +=  "Arguments:\n";

  QMapIterator<QString, QString> it(argsHelp);
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    help +=  "- " + key + getArgsTypeSpecs(cmd, key) + ": " + it.value() + "\n";
  }

  FABRIC_CATCH_END("CommandArgHelpers::createHelpFromArgs");

  return help;
}

QString CommandArgHelpers::createHelpFromRTValArgs(
  QString const&commandHelp,
  QMap<QString, QString> const&argsHelp,
  BaseCommand *cmd)
{
  QString help;
  
  FABRIC_CATCH_BEGIN();

  BaseRTValScriptableCommand *rtValScriptCmd = qobject_cast<BaseRTValScriptableCommand*>(
    cmd);

  if(rtValScriptCmd == 0)
    FabricException::Throw(
      "CommandArgHelpers::createHelpFromArgs",
      "cmd '" + cmd->getName() + "' is not a BaseRTValScriptableCommand");

  help = commandHelp + "\n";

  if(argsHelp.size() > 0)
    help +=  "Arguments:\n";

  QMapIterator<QString, QString> it(argsHelp);
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    if(rtValScriptCmd->hasArg(key)) 
      help += "- " + key + getArgsTypeSpecs(cmd, key) + ": " + it.value() + "\n";
  }
 
  FABRIC_CATCH_END("CommandArgHelpers::createHelpFromRTValArgs");
  
  return help;
}

inline BaseScriptableCommand* CastToBaseScriptableCommand(
  BaseCommand *cmd) 
{
  BaseScriptableCommand* scriptCmd = qobject_cast<BaseScriptableCommand*>(cmd);

  if(scriptCmd == 0)
    FabricException::Throw(
      "CommandArgHelpers::CastToBaseScriptableCommand",
      "Bad cast");

  return scriptCmd;
}

inline BaseRTValScriptableCommand* CastToBaseRTValScriptableCommand(
  BaseCommand *cmd) 
{
  BaseRTValScriptableCommand* scriptCmd = qobject_cast<BaseRTValScriptableCommand*>(cmd);

  if(scriptCmd == 0)
    FabricException::Throw(
      "CommandArgHelpers::CastToBaseRTValScriptableCommand",
      "Bad cast");

  return scriptCmd;
}

bool CommandArgHelpers::isRTValScriptableCommand(
  BaseCommand *cmd)
{
  BaseRTValScriptableCommand* scriptCmd = qobject_cast<BaseRTValScriptableCommand*>(cmd);
  return scriptCmd != 0;
}

bool CommandArgHelpers::isPathValueCommandArg(
  QString const&key,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = CastToBaseScriptableCommand(cmd);
  return scriptCmd->hasArgFlag(key, CommandArgFlags::IO_ARG) ||
      scriptCmd->hasArgFlag(key, CommandArgFlags::IN_ARG) ||
    scriptCmd->hasArgFlag(key, CommandArgFlags::OUT_ARG);
  FABRIC_CATCH_END("CommandArgHelpers::isPathValueCommandArg");
  return false;
}

bool CommandArgHelpers::isScriptableCommand(
  BaseCommand *cmd)
{
  BaseScriptableCommand* scriptCmd = qobject_cast<BaseScriptableCommand*>(cmd);
  return scriptCmd != 0;
}

bool CommandArgHelpers::isCommandArg(
  QString const&key,
  int flags,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = CastToBaseScriptableCommand(cmd);
  return scriptCmd->hasArgFlag(key, flags);
  FABRIC_CATCH_END("CommandArgHelpers::isCommandArg");
  return false;
}

bool CommandArgHelpers::isCommandArgSet(
  QString const&key,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = qobject_cast<BaseScriptableCommand*>(cmd);
  return scriptCmd->isArgSet(key);
  FABRIC_CATCH_END("CommandArgHelpers::isCommandArgSet");
  return false;
}

bool CommandArgHelpers::hasCommandArg(
  QString const&key,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = CastToBaseScriptableCommand(cmd);
  return scriptCmd->hasArg(key);
  FABRIC_CATCH_END("CommandArgHelpers::hasCommandArg");
  return false;
}

QList<QString> CommandArgHelpers::getCommandArgKeys(
  BaseCommand *cmd)
{
  QList<QString> keys;
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = CastToBaseScriptableCommand(cmd);
  keys = scriptCmd->getArgKeys();
  FABRIC_CATCH_END("CommandArgHelpers::getCommandArgKeys");
  return keys;
}

QString CommandArgHelpers::getCommandArg(
  QString const&key,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseScriptableCommand* scriptCmd = CastToBaseScriptableCommand(cmd);
  return scriptCmd->getArg(key);
  FABRIC_CATCH_END("CommandArgHelpers::getCommandArg");
  return "";
}

RTVal CommandArgHelpers::getRTValCommandArg(
  QString const&key,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseRTValScriptableCommand* scriptCmd = CastToBaseRTValScriptableCommand(cmd);
  return scriptCmd->getRTValArg(key);
  FABRIC_CATCH_END("CommandArgHelpers::getRTValCommandArg");
  return RTVal();
}

RTVal CommandArgHelpers::getRTValCommandArgValue(
  QString const&key,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseRTValScriptableCommand* scriptCmd = CastToBaseRTValScriptableCommand(cmd);
  return scriptCmd->getRTValArgValue(key);
  FABRIC_CATCH_END("CommandArgHelpers::getRTValCommandArgValue");
  return RTVal();
}

RTVal CommandArgHelpers::getRTValCommandArgValue(
  QString const&key,
  QString const&type,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseRTValScriptableCommand* scriptCmd = CastToBaseRTValScriptableCommand(cmd);
  return scriptCmd->getRTValArgValue(key, type);
  FABRIC_CATCH_END("CommandArgHelpers::getRTValCommandArgValue");
  return RTVal();
}

QString CommandArgHelpers::getRTValCommandArgType(
  QString const&key,
  BaseCommand *cmd)
{
  FABRIC_CATCH_BEGIN();
  BaseRTValScriptableCommand* scriptCmd = CastToBaseRTValScriptableCommand(cmd);
  return scriptCmd->getRTValArgType(key);
  FABRIC_CATCH_END("CommandArgHelpers::getRTValCommandArgType");
  return "";
}

QString CommandArgHelpers::encodeJSONChars(
  QString const&string)
{
  QString result = string;
  result = result.replace("\"", "'").replace("\\", "\\\\").replace(" ", "");
  return result.replace("\r", "").replace("\n", "").replace("\t", "");
}

QString CommandArgHelpers::encodeJSON(
  QString const&string)
{
  return "\"" + encodeJSONChars(string) + "\"";
}

QString CommandArgHelpers::encodeJSONs(
  QString const&string)
{
  /*
  result = "\""
    for i in range(0, len(strings)):
        if i > 0:
            result += "|"
        result += CommandArgHelpers.__EncodeJSONChars(strings[i])
    result += "\""
    return result
  */
  return "";
}