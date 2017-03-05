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

QString BaseScriptableCommand::getArg(
  const QString &key)
{
  return (m_args.count(key) > 0) ?  m_args[key] : QString();
}

QMap<QString, QString> BaseScriptableCommand::getArgs()
{
  return m_args; 
}
 
void BaseScriptableCommand::setArg(
  const QString &key, 
  const QString &value) 
{
  if(key.isEmpty()) 
    throw(
      std::string("BaseScriptableCommand::setArg, error setting arg: key not specified")
    );

  if(m_argSpecs.count(key) == 0) 
    // TODO: make this an optional behavior
    throw(
      std::string(
        "BaseScriptableCommand::setArg, error setting arg: '" + std::string(key.toUtf8().constData()) + "' not supported by this command"
      )
    );

  m_args.insert(key, value);
}

void BaseScriptableCommand::validateSetArgs()
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
          "BaseScriptableCommand::validateSetArgs, error validating arg: '" + std::string(key.toUtf8().constData()) + "' has not been set"
        )
      );
    }
  }
}

QString BaseScriptableCommand::getDescription() 
{
  QString text = getName();

  // Get the args from the method, since
  // different implementation may exist.
  QMap<QString, QString> args = getArgs();
  if( args.size() > 0 )
  {
    int count = 0;
    QMapIterator<QString, QString> ite(args);

    text += "(";
    while (ite.hasNext()) 
    {
      ite.next();
      text += ite.key();
      text += ":";
      text += ite.value();
      text += (count < args.size() - 1) ? ", " : ")";
    }
  }

  return text;
}

QString BaseScriptableCommand::getArgsDescription() 
{
  QMapIterator<QString, QString> argsIt(m_args);
  QMapIterator<QString, ScriptableCommandArgSpec> argSpecsIt(m_argSpecs);

  int count = 0;
  QString res;
  while (argsIt.hasNext()) 
  {
    argsIt.next();
    argSpecsIt.next();
    ScriptableCommandArgSpec spec = argSpecsIt.value();

    res += "    [" + argsIt.key() 
      + "] opt: " + QString::number(spec.optional)
      + " val: " + argsIt.value()
      + " defVal: " + spec.defaultValue;

    res += (count < m_args.size() - 1) ? "\n" : "";

    count++;
  }

  return res;
}
