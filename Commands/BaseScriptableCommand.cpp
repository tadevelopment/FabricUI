//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

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
    throw(
      std::string("Error declaring arg: key not specified")
    );

  ScriptableCommandArgSpec spec;
  spec.optional = optional;
  spec.defaultValue = defaultValue;
  m_argSpecs.insert(key, spec);

  if(!defaultValue.isEmpty()) 
    setArg(key, defaultValue);
}

QString BaseScriptableCommand::getArg(
  const QString &key) const 
{
  return (m_args.count(key) > 0) ?  m_args[key] : QString();
}

QMap<QString, QString> BaseScriptableCommand::getArgs() const 
{
  return m_args; 
}
 
void BaseScriptableCommand::setArg(
  const QString &key, 
  const QString &value) 
{
  if(key.isEmpty()) 
    throw(
      std::string("Error setting arg: key not specified")
    );

  if(m_argSpecs.count(key) == 0) 
    // TODO: make this an optional behavior
    throw(
      std::string(
        "Error setting arg: '" + std::string(key.toUtf8().constData()) + "' not supported by this command"
      )
    );

  m_args.insert(key, value);
}

void BaseScriptableCommand::validateSetArgs() const
{
  QMapIterator<QString, QString> argsIt(m_args);
  QMapIterator<QString, ScriptableCommandArgSpec> argSpecsIt(m_argSpecs);
  
  while (argSpecsIt.hasNext()) 
  {
    argsIt.next();
    argSpecsIt.next();

    QString key = argSpecsIt.key();
    ScriptableCommandArgSpec spec = argSpecsIt.value();
 
    if(!spec.optional) 
    {
      QString arg = argsIt.value();

      if(arg.isEmpty())//is null
       throw(
        std::string(
          "Error validating arg: '" + std::string(key.toUtf8().constData()) + "' has not been set"
        )
      );
    }
  }
}

QString BaseScriptableCommand::getArgsDescription() const 
{
  QString res;

  QMapIterator<QString, QString> argsIt(m_args);
  QMapIterator<QString, ScriptableCommandArgSpec> argSpecsIt(m_argSpecs);
  while (argsIt.hasNext()) 
  {
    argsIt.next();
    argSpecsIt.next();
    ScriptableCommandArgSpec spec = argSpecsIt.value();

    res += argsIt.key() 
      + " -optional: " + QString::number(spec.optional)
      + " -value: " + argsIt.value()
      + " -defaultValue: " + spec.defaultValue;
  }

  return res;
}

bool BaseScriptableCommand::canUndo() const 
{
  return true;
}

QString BaseScriptableCommand::getHelp() const 
{
  return "";
}
