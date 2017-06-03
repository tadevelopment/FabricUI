//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandArgHelpers.h"
#include "CommandArgHelpers.h"
#include "BaseScriptableCommand.h"
#include "BaseRTValScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>
 
/// Optional argument flag
int FabricUI::Commands::CommandArgFlags::OPTIONAL_ARG = 1;
/// Loggale argument flag
int FabricUI::Commands::CommandArgFlags::LOGGABLE_ARG = 2;
/// Input argument flag
int FabricUI::Commands::CommandArgFlags::IN_ARG = 4;
/// Output argument flag
int FabricUI::Commands::CommandArgFlags::OUT_ARG = 8;
/// IO argument flag
int FabricUI::Commands::CommandArgFlags::IO_ARG = 16;

using namespace FabricUI;
using namespace Commands;
using namespace Application;

QString CommandArgHelpers::CreateHistoryDescFromArgs(
  const QMap<QString, QString> &argsDesc,
  BaseScriptableCommand *cmd)
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
      QString argDesc = (rtValScriptCmd && !rtValScriptCmd->getRTValArgPath(key).isEmpty())
        ? argDesc = "<" + it.value() + ">"
        : it.value();

      desc += key + "=\"" + argDesc + "\"";
      if(count < argsDesc.size()-1)
        desc += ", ";
      count++;
    }

    desc +=  ")";
  }

  return desc;

  FABRIC_CATCH_END("CommandArgHelpers::CreateHistoryDescFromArgs");

  return "";
}

QString CommandArgHelpers::CreateHelpFromArgs(
  const QString &commandHelp,
  const QMap<QString, QString> &argsHelp,
  BaseScriptableCommand *cmd)
{
  FABRIC_CATCH_BEGIN();

  QString help = commandHelp + "\n";

  if(argsHelp.size() > 0)
    help +=  "Arguments:\n";

  QMapIterator<QString, QString> it(argsHelp);
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    QString argHelp = it.value();

    QString specs; 
    if(cmd->isArg(key, CommandArgFlags::OPTIONAL_ARG) || cmd->isArg(key, CommandArgFlags::LOGGABLE_ARG))
    {
      specs += "["; 

      if(cmd->isArg(key, CommandArgFlags::OPTIONAL_ARG))
        specs += "optional"; 

      if(cmd->isArg(key, CommandArgFlags::LOGGABLE_ARG))
      {
        if(cmd->isArg(key, CommandArgFlags::OPTIONAL_ARG))
          specs += ", loggable"; 
        specs += "loggable"; 
      }

      specs += "]"; 
    }
     
    help +=  "- " + key + specs + ": " + argHelp + "\n";
  }

  return help;

  FABRIC_CATCH_END("CommandArgHelpers::CreateHelpFromArgs");

  return "";
}

QString CommandArgHelpers::CreateHelpFromRTValArgs(
  const QString &commandHelp,
  const QMap<QString, QString> &argsHelp,
  BaseRTValScriptableCommand *cmd)
{
  FABRIC_CATCH_BEGIN();

  QString help = commandHelp + "\n";

  if(argsHelp.size() > 0)
    help +=  "Arguments:\n";

  QMapIterator<QString, QString> it(argsHelp);
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();

    if(!cmd->hasArg(key)) 
      return "";

    QString argHelp = it.value();

    QString specs; 
    specs += " ["; 

    if( cmd->isArg(key, CommandArgFlags::IN_ARG) ||
        cmd->isArg(key, CommandArgFlags::OUT_ARG) ||
        cmd->isArg(key, CommandArgFlags::IO_ARG) )
      specs += "PathValue[" + cmd->getRTValArgType(key)+"]"; 
    else
      specs += cmd->getRTValArgType(key); 

    if(cmd->isArg(key, CommandArgFlags::OPTIONAL_ARG))
      specs += ", optional"; 

    if(cmd->isArg(key, CommandArgFlags::LOGGABLE_ARG))
      specs += ", loggable"; 

    if(cmd->isArg(key, CommandArgFlags::IN_ARG))
      specs += ", IN"; 

    if(cmd->isArg(key, CommandArgFlags::OUT_ARG))
      specs += ", OUT"; 

    if(cmd->isArg(key, CommandArgFlags::IO_ARG))
      specs += ", IO"; 
    
    specs += "]"; 
     
    help +=  "- " + key + specs + ": " + argHelp + "\n";
  }
 
  return help;
  
  FABRIC_CATCH_END("CommandArgHelpers::CreateHelpFromRTValArgs");

  return "";
}
