//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandArgHelpers.h"
#include "BaseScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Commands;
using namespace Application;

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
    FabricException::Throw(
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
    FabricException::Throw(
      "BaseScriptableCommand::isArg",
      "setting arg of '" + getName() + "', key not specified");

  if(!hasArg(key)) 
    // TODO: make this an optional behavior
    FabricException::Throw(
      "BaseScriptableCommand::isArg",
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
    FabricException::Throw(
      "BaseScriptableCommand::setArg",
      "setting arg of '" + getName() + "', key not specified");

  if(!hasArg(key)) 
    // TODO: make this an optional behavior
    FabricException::Throw(
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
     
    if(!isArg(key, CommandArgFlags::OPTIONAL_ARG) && !isArgSet(key)) //is null
      FabricException::Throw(
        "BaseScriptableCommand::validateSetArgs",
        "validating arg: '" + key + "' of command '" + getName() + "' has not been set");
  }
}

QString BaseScriptableCommand::getArgsDescription() 
{
  QString res;

  FABRIC_CATCH_BEGIN();

  int count = 0;
  QMapIterator<QString, ScriptableCommandArgSpec> it(m_argSpecs);
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    ScriptableCommandArgSpec spec = it.value();

    res += "    ["  + key + "]";
    res += ", opt: " + CommandArgHelpers::getArgsTypeSpecs(this, key);
    res +=  ", val: " + getArg(key);

    if(!spec.defaultValue.isEmpty())
      res += ", defVal: " + spec.defaultValue;
 
    res += (count < m_argSpecs.size() - 1) ? "\n" : "";
    count++;
  }

  FABRIC_CATCH_END("BaseScriptableCommand::getArgsDescription");

  return res;
}
