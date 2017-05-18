//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QStringList>
#include "CommandException.h"
#include "RTValCommandManager.h"
#include <FabricUI/Util/RTValUtil.h>
#include "BaseRTValScriptableCommand.h"

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;

BaseRTValScriptableCommand::BaseRTValScriptableCommand() 
  : BaseScriptableCommand()
{
}

BaseRTValScriptableCommand::~BaseRTValScriptableCommand() 
{
}

inline RTValCommandManager *GetManager() {
  return dynamic_cast<RTValCommandManager*>(
    CommandManager::GetCommandManager());
}
 
inline bool IsKnownRTValType(
  const QString &type) 
{
  return type != "RTVal";
}

inline QString GetMainKey(
  const QString &key)
{ 
  return key.indexOf(".") > -1
    ? key.split(".")[0]
    : key;
}

// ScriptableCommand
void BaseRTValScriptableCommand::declareArg(
  const QString &key, 
  int flags, 
  const QString &defaultValue)
{ 
  declareRTValArg(key, "RTVal", flags); 
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
      "Arg: '" + key + "' not supported by command '" + getName() + "'");

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
  return m_rtvalArgs.count(key) && 
    ( m_rtvalArgs[key].first.isValid() ||
      !m_rtvalArgs[key].second.isEmpty() );
}

QString BaseRTValScriptableCommand::getArg(
  const QString &key)
{
  if(!hasArg(key)) 
    CommandException::Throw(  
      "BaseRTValScriptableCommand::getArg",
      "No arg named '" + key + "' in command '" + getName() + "'");

  // Known RTVal of known type, get the json from it.
  return (m_rtvalArgs[key].second.isEmpty() && IsKnownRTValType(m_rtvalArgSpecs[key].type))
    ? RTValUtil::forceRTValToJSON(getRTValArg(key))
    // Otherwise, return the Json if it's been set.
    // It happens if the arg's been declared with an unknown type
    : m_rtvalArgs[key].second;
}

void BaseRTValScriptableCommand::setArg(
  const QString &key, 
  const QString &json) 
{
  QString mainKey = GetMainKey(key);

  if(!hasArg(mainKey)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::setArg",
      "No arg named " + mainKey + "' in command '" + getName() + "'");

  try
  {
    QString complexArgType;
    int flags = m_rtvalArgSpecs[mainKey].flags;

    if( GetManager()->getComplexArgRegistry().getRTValComplexArgType(
          flags, 
          complexArgType) && 
        GetManager()->getComplexArgRegistry().isRTValComplexArgTypeOf(
          flags, 
          json)
      )
    {
      RTVal rtVal = RTValUtil::forceJSONToRTVal(
        GetManager()->getClient(),
        json,
        complexArgType);

      setRTValArg(mainKey, rtVal);
    }

    // Known type, cast the JSON to a RTVal.
    else if(IsKnownRTValType(m_rtvalArgSpecs[mainKey].type))
    {      
      RTVal rtVal = RTValUtil::forceJSONToRTVal(
        GetManager()->getClient(),
        json,
        m_rtvalArgSpecs[mainKey].type);

      setRTValArg(key, rtVal);
    }

    // Store the JSON directly.
    else
      m_rtvalArgs[mainKey].second = json;
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::setArg",
      "",
      e.getDesc_cstr());
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::setArg",
      "",
      e.what());
  }
}

void BaseRTValScriptableCommand::validateSetArgs()
{
  QMapIterator<QString, ScriptableCommandRTValArgSpec> it(m_rtvalArgSpecs);
  
  while(it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    ScriptableCommandRTValArgSpec spec = it.value();

    QString mainKey = GetMainKey(key);
 
    // We support unknown type.
    if(!isArg(mainKey, CommandArgFlags::OPTIONAL_ARG) && !isArgSet(mainKey))
      CommandException::Throw(
        "BaseRTValScriptableCommand::validateSetArgs",
        "Argument '" + mainKey + "' in command '" + getName() + "' has not been set");
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
      + "] opt: "   + QString::number(isArg(key, CommandArgFlags::OPTIONAL_ARG))
      + " val: "    + getArg(key)
      + " defVal: " + spec.defaultValue;

    res += (count < m_rtvalArgs.size() - 1) ? "\n" : "";

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
 
  if(!type.isEmpty() && !GetManager()->getClient().isValidType(type.toUtf8().constData()))
    CommandException::Throw(
      "BaseRTValScriptableCommand::declareRTValArg", 
      "Type '" + type  + "' of command '" + getName() + "' is not a valid KL type");

  try
  {
    ScriptableCommandRTValArgSpec spec;
    spec.type = type;
    spec.flags = flags;

    QString complexArgType;
    spec.defaultValue = GetManager()->getComplexArgRegistry().getRTValComplexArgType(
        flags, 
        complexArgType)
      ? RTVal::Construct(
          GetManager()->getClient(), 
          complexArgType.toUtf8().constData(), 
          0, 
          0)
      : defaultValue;
   
    m_rtvalArgSpecs.insert(key, spec);

    if(!type.isEmpty() && spec.defaultValue.isValid())
    {
      QPair<FabricCore::RTVal, QString> pair;
      pair.first = spec.defaultValue;
      m_rtvalArgs.insert(key, pair);
    }
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::declareRTValArg",
      "",
      e.getDesc_cstr());
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::declareRTValArg",
      "",
      e.what());
  }
}

QString BaseRTValScriptableCommand::getRTValArgType(
  const QString &key)
{
  QString mainKey = GetMainKey(key);

  if(!hasArg(mainKey)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::getRTValArgType",
      "No arg named '" + mainKey + "' in command '" + getName() + "'");

  QString complexArgType;
  int flags = m_rtvalArgSpecs[mainKey].flags;

  if(GetManager()->getComplexArgRegistry().getRTValComplexArgType(
      flags, 
      complexArgType)
    )
    return GetManager()->getComplexArgRegistry().getRTValComplexArgValueType(
      flags,
      key, 
      m_rtvalArgSpecs[mainKey].type);
    
  return m_rtvalArgSpecs[mainKey].type;
}

void BaseRTValScriptableCommand::setRTValArgType(
  const QString &key,
  const QString &type)
{
  QString mainKey = GetMainKey(key);

  if(!hasArg(mainKey)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::setRTValArgType",
      "No arg named '" + mainKey + "' in command '" + getName() + "'");
 
  if(!GetManager()->getClient().isValidType(type.toUtf8().constData()))
    CommandException::Throw(
      "BaseDFGCommand::setRTValArgType",
      "Argument '" + mainKey + "' in command '" + getName() + "' has not a valid kl type '" + type + "'");
 
  try
  {
     // If the type is unknown, set it
    if(!IsKnownRTValType(m_rtvalArgSpecs[mainKey].type))
      m_rtvalArgSpecs[mainKey].type = type;
 
    // If the arg has been set in JSON,  
    // construct it since we know its type
    if(!m_rtvalArgs[mainKey].second.isEmpty())
    {
      RTVal val = RTValUtil::forceJSONToRTVal(
        GetManager()->getClient(),
        m_rtvalArgs[mainKey].second,
        m_rtvalArgSpecs[mainKey].type);

      setRTValArg(key, val);
    }
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::setRTValArgType",
      "",
      e.getDesc_cstr());
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::setRTValArgType",
      "",
      e.what());
  }
}

RTVal BaseRTValScriptableCommand::getRTValArg(
  const QString &key)
{
  QString mainKey = GetMainKey(key);

  if(!hasArg(mainKey)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "No arg named '" + mainKey + "' in command '" + getName() + "'");
  
  if(!m_rtvalArgs[mainKey].first.isValid() && !m_rtvalArgs[mainKey].second.isEmpty())
    CommandException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
        "RTVal argument '" + mainKey + "' of command '" + getName() + 
        "' has been set in JSON only, \n !!! use getRTValArg(mainKey, type) instead !!!");

  RTVal val;

  try
  {
    QString complexArgType;
    int flags = m_rtvalArgSpecs[mainKey].flags;

    val = GetManager()->getComplexArgRegistry().getRTValComplexArgType(
        flags, 
        complexArgType)
      ? GetManager()->getComplexArgRegistry().getRTValComplexArgValue(
          flags, 
          key, 
          RTValUtil::forceToRTVal(m_rtvalArgs[mainKey].first))
      : m_rtvalArgs[mainKey].first;

    val = RTValUtil::forceToRTVal(val);
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "",
      e.getDesc_cstr());
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "",
      e.what());
  }

  return val;
}

RTVal BaseRTValScriptableCommand::getRTValArg(
  const QString &key,
  const QString &type)
{
  setRTValArgType(key, type);
  return getRTValArg(key);
}

void BaseRTValScriptableCommand::setRTValArg(
  const QString &key, 
  RTVal value) 
{ 
  QString mainKey = GetMainKey(key);

  if(!hasArg(mainKey)) 
    CommandException::Throw(
      "BaseRTValScriptableCommand::setRTValArg",
      "No arg named '" + mainKey + "' in command '" + getName() + "'");

  try
  {
    QPair<FabricCore::RTVal, QString> pair;

    // Sets the RTVal.
    QString complexArgType;
    int flags = m_rtvalArgSpecs[mainKey].flags;

    if(GetManager()->getComplexArgRegistry().getRTValComplexArgType(
        flags, 
        complexArgType)
      )
    {
      RTVal complexArg = isArgSet(mainKey)
        ? RTValUtil::forceToRTVal(
            m_rtvalArgs[mainKey].first)
        : RTVal::Construct(
            GetManager()->getClient(), 
            complexArgType.toUtf8().constData(), 
            0, 0);

      QString type = GetManager()->getComplexArgRegistry().setRTValComplexArgValue(
        flags,
        key, 
        value, 
        complexArg);
      
      if(!type.isEmpty() && !IsKnownRTValType(m_rtvalArgSpecs[mainKey].type))
        m_rtvalArgSpecs[mainKey].type = type;

      pair.first = complexArg;
    }

    else
    {
      pair.first = value;

      if(!IsKnownRTValType(m_rtvalArgSpecs[mainKey].type))
        m_rtvalArgSpecs[mainKey].type = RTValUtil::getRTValType(
          value);
    }

    m_rtvalArgs.insert(mainKey, pair);
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::setRTValArg",
      "",
      e.getDesc_cstr());
  }

  catch(CommandException &e) 
  {
    CommandException::Throw(
      "BaseRTValScriptableCommand::setRTValArg",
      "",
      e.what());
  }
}
