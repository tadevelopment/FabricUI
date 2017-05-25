//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandArgHelpers.h"
#include <FabricUI/Application/FabricException.h>

namespace FabricUI {
namespace Commands {
 
QString CreateHistoryDescFromArgs(
  const QMap<QString, QString> &argsDesc,
  BaseScriptableCommand *cmd)
{
  QString desc;

  try
  {
    desc = cmd->getName();

    if(argsDesc.size() > 0)
    {
      int count = 0;
      desc +=  "(";

      QMapIterator<QString, QString> it(argsDesc);
      while(it.hasNext()) 
      {
        it.next();
        QString key = it.key();
        QString argDesc = it.value();
        desc += key + "=\"" + argDesc + "\"";
        if(count < argsDesc.size()-1)
          desc += ", ";
        count++;
      }

      desc +=  ")";
    }
  }
 
  catch(Application::FabricException &e) 
  {
    Application::FabricException::Throw(
      "CommandArgHelpers::CreateHistoryDescFromArgs",
      "",
      e.what());
  }

  return desc;
}

QString CreateHelpFromArgs(
  const QString &commandHelp,
  const QMap<QString, QString> &argsHelp,
  BaseScriptableCommand *cmd)
{
  QString help;

  try
  {
    help = commandHelp + "\n";

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
  }
 
  catch(Application::FabricException &e) 
  {
    Application::FabricException::Throw(
      "CommandArgHelpers::CreateHelpFromArgs",
      "",
      e.what());
  }

  return help;
}

QString CreateHelpFromRTValArgs(
  const QString &commandHelp,
  const QMap<QString, QString> &argsHelp,
  BaseRTValScriptableCommand *cmd)
{
  QString help;

  try
  {
    help = commandHelp + "\n";

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
        specs += "PathValue[" + cmd->getRTValArgType(key+".value")+"]"; 
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
  }
 
  catch(Application::FabricException &e) 
  {
    Application::FabricException::Throw(
      "CommandArgHelpers::CreateHelpFromRTValArgs",
      "",
      e.what());
  }

  return help;
}

} // namespace Commands
} // namespace FabricUI
