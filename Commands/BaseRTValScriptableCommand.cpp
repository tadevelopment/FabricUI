//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandException.h"
#include "RTValCommandManager.h"
#include <FabricUI/Util/RTValUtil.h>
#include "BaseRTValScriptableCommand.h"

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Util;

BaseRTValScriptableCommand::BaseRTValScriptableCommand() 
  : BaseScriptableCommand()
{
}

BaseRTValScriptableCommand::~BaseRTValScriptableCommand() 
{
}

// ScriptableCommand
void BaseRTValScriptableCommand::declareArg(
  const QString &key, 
  int flags, 
  const QString &defaultValue)
{ 
  if(key.isEmpty()) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::declareArg",
      "Key not specified  in command '" + getName() + "'");
  
  ScriptableCommandRTValArgSpec spec;
  spec.type = QString();
  spec.flags = flags;
  m_rtvalArgSpecs.insert(key, spec);
}

bool BaseRTValScriptableCommand::hasArg(
  const QString &key)
{
  return m_rtvalArgSpecs.count(key) > 0;
}

bool BaseRTValScriptableCommand::isArg(
  const QString &key,
  int flag)
{
  if(key.isEmpty()) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::isArg",
      "setting arg of '" + getName() + "', key not specified");

  if(!hasArg(key)) 
    // TODO: make this an optional behavior
    CommandException::Throw(
      "BaseRTValScriptableCommand::isArg",
      "setting arg: '" + key + + "' not supported by command '" + getName() + "'");

  return (m_rtvalArgSpecs[key].flags & flag);
}

QList<QString> BaseRTValScriptableCommand::getArgKeys()
{
  return m_rtvalArgSpecs.keys(); 
}

bool BaseRTValScriptableCommand::isArgSet(
  const QString &key)
{
  // The arg can be set as RTVal or as JSON.
  return BaseScriptableCommand::isArgSet(key) || 
    (m_rtvalArgs.count(key) && m_rtvalArgs[key].isValid());
}

QString BaseRTValScriptableCommand::getArg(
  const QString &key)
{
  if(!hasArg(key)) 
    CommandException::Throw(  
      "BaseRTValScriptableCommand::getArg",
      "No arg named '" + key + "' in command '" + getName() + "'");

  // Known RTVal of known type, get the json from it.
  return (m_rtvalArgs.count(key) > 0 && !m_rtvalArgSpecs[key].type.isEmpty())
    ? RTValUtil::forceRTValToJSON(getRTValArg(key))
    // Otherwise, return the Json if it's been set.
    // It happens if the arg's been declared with an unknown type
    : m_args.count(key) > 0 ? m_args[key] : QString();
}

void BaseRTValScriptableCommand::setArg(
  const QString &key, 
  const QString &json) 
{
  if(!hasArg(key)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::setArg",
      "No arg named " + key + "' in command '" + getName() + "'");

  // Known type, cast the JSON to a RTVal.
  if(!m_rtvalArgSpecs[key].type.isEmpty())
  {
    RTValCommandManager *manager = dynamic_cast<RTValCommandManager*>(
      CommandManager::GetCommandManager());

    RTVal rtVal = RTValUtil::forceJSONToRTVal(
      manager->getClient(),
      json,
      m_rtvalArgSpecs[key].type);

    setRTValArg(key, rtVal);
  }

  // Store the JSON directly.
  else 
    m_args.insert(key, json);
}

void BaseRTValScriptableCommand::validateSetArgs()
{
  QMapIterator<QString, ScriptableCommandRTValArgSpec> it(m_rtvalArgSpecs);
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    ScriptableCommandRTValArgSpec spec = it.value();

    // We support unknown type.
    if(!isArg(key, CommandFlags::OPTIONAL_ARG) && !isArgSet(key))
      CommandException::Throw(
        "BaseRTValScriptableCommand::validateSetArgs",
        "Argument '" + key + "' in command '" + getName() + "' has not been set");
  }
}

QString BaseRTValScriptableCommand::getArgsDescription() 
{
  QMapIterator<QString, ScriptableCommandRTValArgSpec> it(m_rtvalArgSpecs);

  int count = 0;
  QString res;
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    ScriptableCommandRTValArgSpec spec = it.value();

    res += "    ["  + key 
      + "] opt: "   + QString::number(isArg(key, CommandFlags::OPTIONAL_ARG))
      + " val: "    + getArg(key)
      + " defVal: " + spec.defaultValue;

    res += (count < m_args.size() - 1) ? "\n" : "";

    count++;
  }

  return res;
}

// RTValScriptableCommand
void BaseRTValScriptableCommand::declareRTValArg(
  const QString &key, 
  const QString &type,
  int flags, 
  RTVal defaultValue) 
{
  if(key.isEmpty()) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::declareRTValArg",
      "Key not specified  in command '" + getName() + "'");

  RTValCommandManager *manager = dynamic_cast<RTValCommandManager*>(
    CommandManager::GetCommandManager());

  if(!type.isEmpty() && !manager->getClient().isValidType(type.toUtf8().constData()))
    CommandException::Throw(
      "BaseRTValScriptableCommand::declareRTValArg", 
      "Type '" + type  + "' of command '" + getName() + "' is not a valid KL type");

  ScriptableCommandRTValArgSpec spec;
  spec.type = type;
  spec.flags = flags;
  m_rtvalArgSpecs.insert(key, spec);

  if(!type.isEmpty() && defaultValue.isValid())
  {
    m_rtvalArgSpecs[key].defaultValue = defaultValue;
    setRTValArg(key, defaultValue);
  }
}

QString BaseRTValScriptableCommand::getRTValArgType(
  const QString &key)
{
  if(!hasArg(key)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::getRTValArgType",
      "No arg named '" + key + "' in command '" + getName() + "'");

  return m_rtvalArgSpecs[key].type;
}

void BaseRTValScriptableCommand::setRTValArgType(
  const QString &key,
  const QString &type)
{
  if(!hasArg(key)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::setRTValArgType",
      "No arg named '" + key + "' in command '" + getName() + "'");

  RTValCommandManager *manager = dynamic_cast<RTValCommandManager*>(
    CommandManager::GetCommandManager());

  if(!manager->getClient().isValidType(type.toUtf8().constData()))
    CommandException::Throw(
      "BaseRTValScriptableCommand::setRTValArgType",
      "Argument '" + key + "' in command '" + getName() + "' has not a valid kl type '" + type + "'");

  // If the type is different, update it
  if(m_rtvalArgSpecs[key].type != type)
  {
    m_rtvalArgSpecs[key].type = type;

    // If the arg has been set in JSON,  
    // construct it since we know its type
    if(m_args.count(key) > 0)
    {
      RTVal rtVal = RTValUtil::forceJSONToRTVal(
        manager->getClient(),
        m_args[key],
        type);
        
      setRTValArg(
        key, 
        rtVal);
    }
  }
}

RTVal BaseRTValScriptableCommand::getRTValArg(
  const QString &key)
{
  if(!hasArg(key)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "No arg named '" + key + "' in command '" + getName() + "'");

  if(m_rtvalArgs.count(key) == 0)
  {
    if(m_args.count(key) > 0)
      CommandException::Throw(
        "BaseRTValScriptableCommand::getRTValArg",
          "RTVal argument '" + key + "' of command '" + getName() + 
          "' has bot been set, but its JSON is, \n !!! use getRTValArg(key, type) instead !!!");

    else
      CommandException::Throw(
        "BaseRTValScriptableCommand::getRTValArg",
        "Argument '" + key + "' of command '" + getName() + "' has bot been set");
  }

  return RTValUtil::forceToRTVal(
      m_rtvalArgs[key]);
}

RTVal BaseRTValScriptableCommand::getRTValArg(
  const QString &key,
  const QString &type)
{
  if(!hasArg(key)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "No arg named '" + key + "' in command '" + getName() + "'");

  // The RTVal's been set in JSON -> construct it.
  if(m_rtvalArgs.count(key) == 0) 
    setRTValArgType(
      key,
      type);

  return getRTValArg(key);
}

void BaseRTValScriptableCommand::setRTValArg(
  const QString &key, 
  RTVal rtVal) 
{
  if(!hasArg(key)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::setRTValArg",
      "No arg named '" + key + "' in command '" + getName() + "'");

  // Sets the RTVal.
  m_rtvalArgs.insert(key, rtVal);

  // Sets its type if we doesn't know it already.
  // It happens if the arg's been declared with an unknown type.
  if(m_rtvalArgSpecs[key].type.isEmpty())
    m_rtvalArgSpecs[key].type = RTValUtil::getRTValType(
      rtVal);
}

QString BaseRTValScriptableCommand::createHelpFromArgs(
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

    if(!hasArg(key)) 
      return "";

    QString argHelp = it.value();

    QString specs; 
    specs += " ["; 
    specs += m_rtvalArgSpecs[key].type; 

    if(isArg(key, CommandFlags::OPTIONAL_ARG) || isArg(key, CommandFlags::LOGGABLE_ARG))
    {
      if(isArg(key, CommandFlags::OPTIONAL_ARG))
        specs += ", optional"; 

      if(isArg(key, CommandFlags::LOGGABLE_ARG))
      {
        if(isArg(key, CommandFlags::OPTIONAL_ARG))
          specs += ", loggable"; 
        specs += ", loggable"; 
      }
    }
    specs += "]"; 
     
    help +=  "- " + key + specs + ": " + argHelp + "\n";
  }

  return help;
}
