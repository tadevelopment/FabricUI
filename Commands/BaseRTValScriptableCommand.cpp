//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandManager.h"
#include "BaseRTValScriptableCommand.h"
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

BaseRTValScriptableCommand::BaseRTValScriptableCommand() 
  : BaseScriptableCommand()
{
}

BaseRTValScriptableCommand::~BaseRTValScriptableCommand() 
{
}

void BaseRTValScriptableCommand::declareArg(
  const QString &key, 
  const QString &type,
  bool optional, 
  const QString &defaultValue) 
{
  BaseScriptableCommand::declareArg(
    key,
    optional,
    defaultValue);

  setArgType(key, type);
}

void BaseRTValScriptableCommand::declareArg(
  const QString &key, 
  const QString &type,
  bool optional, 
  RTVal defaultValue) 
{
  Client client = 
    Commands::CommandManager::GetCommandManager()->getFabricClient();

  QString defaultValueStr = Util::RTValUtil::rtValToJSON(
    client,
    defaultValue
  );

  BaseScriptableCommand::declareArg(
    key,
    optional,
    defaultValueStr);

  setArgType(key, type);
}
 
void BaseRTValScriptableCommand::setArgType(
  const QString &key, 
  const QString &type) 
{
  if(m_argSpecs.count(key) == 0) 
    throw(
      std::string(
        "BaseRTValScriptableCommand::setArgType, error no arg named " + 
        std::string(type.toUtf8().constData())
      )
    );

  m_argSpecs[key].type = type;
}

RTVal BaseRTValScriptableCommand::getArgAsRTVal(
  const QString &key)
{
  RTVal res;

  try
  {
    if(m_args.count(key) > 0)
      res = Util::RTValUtil::jsonToRTVal(
        Commands::CommandManager::GetCommandManager()->getFabricClient(),
        getArg(key),
        m_argSpecs[key].type);
  }

  catch(Exception &e)
  {
    printf(
      "BaseRTValScriptableCommand::getArgAsRTVal: exception: %s\n", 
      e.getDesc_cstr());
  }

  return res;
}

QMap<QString, RTVal> BaseRTValScriptableCommand::getArgsAsRTVal()
{
  QMap<QString, RTVal> res;
  
  QMapIterator<QString, QString> argsIt(m_args);
  while (argsIt.hasNext()) 
  {
    argsIt.next();
    QString argName = argsIt.key();
    res[argName] = getArgAsRTVal(argName);
  }

  return res;
}

QString BaseRTValScriptableCommand::getDescription() 
{
  QString text = "Commands.";
  text += getName();

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
      text += "=";
      text += Util::RTValUtil::EncodeString( ite.value() ); 
      text += (count < args.size() - 1) 
        ? ", " 
        : ")";
      count ++;
    }
  }

  return text;
}
