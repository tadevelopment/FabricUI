//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandException.h"
#include "BaseScriptableCommand.h"

using namespace FabricUI;
using namespace Commands;

BaseScriptableCommand::BaseScriptableCommand() 
  : BaseCommand()
{
}

BaseScriptableCommand::~BaseScriptableCommand() 
{
}

void BaseScriptableCommand::declareArg(
  const QString &key, 
  int flags, 
  const QString &defaultValue) 
{
  if(key.isEmpty())
    CommandException::Throw(
      "BaseScriptableCommand::declareArg",
      "declaring arg of '" + getName() + "', key not specified");

  ScriptableCommandArgSpec spec;
  spec.flags = flags;
  spec.defaultValue = defaultValue;
  m_argSpecs[key] = spec;

  if(!defaultValue.isEmpty())
    setArg(key, defaultValue);
}

bool BaseScriptableCommand::hasArg(
  const QString &key)
{
  return m_argSpecs.count(key) > 0;
}

bool BaseScriptableCommand::isArg(
  const QString &key,
  int flag)
{
  if(key.isEmpty()) 
    CommandException::Throw(
      "BaseScriptableCommand::isArgOptional",
      "setting arg of '" + getName() + "', key not specified");

  if(!hasArg(key)) 
    // TODO: make this an optional behavior
    CommandException::Throw(
      "BaseScriptableCommand::isArgOptional",
      "setting arg: '" + key + + "' not supported by command '" + getName() + "'");

  return (m_argSpecs[key].flags & flag);
}

QString BaseScriptableCommand::getArg(
  const QString &key)
{
  return m_args.count(key) > 0 
    ? m_args[key]
    : QString();
}

QList<QString> BaseScriptableCommand::getArgKeys()
{
  return m_argSpecs.keys(); 
}

bool BaseScriptableCommand::isArgSet(
  const QString &key)
{
  return m_args.count(key) && 
        !m_args[key].isEmpty();
}

void BaseScriptableCommand::setArg(
  const QString &key, 
  const QString &value) 
{
  if(key.isEmpty()) 
    CommandException::Throw(
      "BaseScriptableCommand::setArg",
      "setting arg of '" + getName() + "', key not specified");

  if(!hasArg(key)) 
    // TODO: make this an optional behavior
    CommandException::Throw(
      "BaseScriptableCommand::setArg",
      "setting arg: '" + key + + "' not supported by command '" + getName() + "'");

  m_args.insert(key, value);
}

void BaseScriptableCommand::validateSetArgs()
{
  QMapIterator<QString, ScriptableCommandArgSpec> it(m_argSpecs);
  
  while(it.hasNext()) 
  {
    it.next();

    QString key = it.key();
    ScriptableCommandArgSpec spec = it.value();
     
    if(!isArg(key, CommandFlags::OPTIONAL_ARG) && !isArgSet(key)) //is null
      CommandException::Throw(
        "BaseScriptableCommand::validateSetArgs",
        "validating arg: '" + key + "' of command '" + getName() + "' has not been set");
  }
}

QString BaseScriptableCommand::getArgsDescription() 
{
  QMapIterator<QString, ScriptableCommandArgSpec> it(m_argSpecs);

  int count = 0;
  QString res;
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    ScriptableCommandArgSpec spec = it.value();

    res += "    ["  + key 
      + "] opt: "   + QString::number(isArg(key, CommandFlags::OPTIONAL_ARG))
      + " val: "    + getArg(key)
      + " defVal: " + spec.defaultValue;

    res += (count < m_args.size() - 1) ? "\n" : "";

    count++;
  }

  return res;
}

QString BaseScriptableCommand::createHelpFromArgs(
  const QString &commandHelp,
  const QMap<QString, QString> &argsHelp)
{
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
    if(isArg(key, CommandFlags::OPTIONAL_ARG) || isArg(key, CommandFlags::LOGGABLE_ARG))
    {
      specs += "["; 

      if(isArg(key, CommandFlags::OPTIONAL_ARG))
        specs += "optional"; 

      if(isArg(key, CommandFlags::LOGGABLE_ARG))
      {
        if(isArg(key, CommandFlags::OPTIONAL_ARG))
          specs += ", loggable"; 
        specs += "loggable"; 
      }

      specs += "]"; 
    }
     
    help +=  "- " + key + specs + ": " + argHelp + "\n";
  }

  return help;
}

QString BaseScriptableCommand::createHistoryDescFromArgs(
  const QMap<QString, QString> &argsDesc)
{
  QString desc = getName();

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

  return desc;
}
