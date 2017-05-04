//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandHelpers.h"
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
  bool optional, 
  const QString &defaultValue) 
{
  if(key.isEmpty())
  {
    printf("BaseScriptableCommand::declareArg, error declaring arg: key not specified");
    return;
  }

  ScriptableCommandArgSpec spec;
  spec.optional = optional;
  spec.defaultValue = defaultValue;
  m_argSpecs.insert(key, spec);

  if(!defaultValue.isEmpty())
    setArg(key, defaultValue);
}

bool BaseScriptableCommand::hasArg(
  const QString &key)
{
  return m_argSpecs.count(key) > 0;
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

void BaseScriptableCommand::setArg(
  const QString &key, 
  const QString &value) 
{
  if(key.isEmpty()) 
    printAndThrow(
      "BaseScriptableCommand::setArg, error setting arg: key not specified"
    );

  if(m_argSpecs.count(key) == 0) 
    // TODO: make this an optional behavior
    printAndThrow(
      std::string(
        "BaseScriptableCommand::setArg, error setting arg: '" + 
        std::string(key.toUtf8().constData()) + "' not supported by this command"
      )
    );

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
 
    bool isValid = !m_args.count(key) && m_args[key].isEmpty();

    if(!spec.optional && !isValid) //is null
      printAndThrow(
        std::string(
          "BaseScriptableCommand::validateSetArgs, error validating arg: '" + 
          std::string(key.toUtf8().constData()) + "' has not been set"
        )
      );
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
      + "] opt: "   + QString::number(spec.optional)
      + " val: "    + getArg(key)
      + " defVal: " + spec.defaultValue;

    res += (count < m_args.size() - 1) ? "\n" : "";

    count++;
  }

  return res;
}
