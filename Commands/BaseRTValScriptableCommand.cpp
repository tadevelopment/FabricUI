//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QStringList>
#include <FabricUI/Util/RTValUtil.h>
#include "BaseRTValScriptableCommand.h"
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

BaseRTValScriptableCommand::BaseRTValScriptableCommand() 
  : BaseScriptableCommand()
{
}

BaseRTValScriptableCommand::~BaseRTValScriptableCommand() 
{
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

inline QString ParsePathValueArgKey(
  const QString &key)
{ 
  return key.indexOf(".") > -1
    ? key.split(".")[1]
    : key;
}

inline QString GetPathValueArgType(
  const QString &key, 
  const QString &type) 
{
  return (ParsePathValueArgKey(key) == "path")
    ? "String"
    : type;
}

inline bool IsPathValueArg(
  int flags)
{ 
  return (flags & CommandArgFlags::IN_ARG) || 
    (flags & CommandArgFlags::OUT_ARG) || 
    (flags & CommandArgFlags::IO_ARG);
}

inline QString SetPathValueArg(
  const QString &key, 
  RTVal value, 
  RTVal &pathValue) 
{
  QString type;

  try
  {
    QString subKey = ParsePathValueArgKey(key);

    if(subKey == "path")
      pathValue.setMember("path", value);
    
    else if(subKey == "value" || RTValUtil::getType(value) != "PathValue")
    {
      pathValue.setMember("value", value);
      type = RTValUtil::getType(value);
    }

    else
    {
      pathValue = value;
      type = RTValUtil::getType(
        RTValUtil::toRTVal(value).maybeGetMember("value"));
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::setValue",
      "",
      e.getDesc_cstr());
  }

  return type;
}

inline RTVal GetPathValueArg(
  const QString &key,
  RTVal pathValue)
{
  RTVal res;

  try
  {
    QString subKey = ParsePathValueArgKey(key);
        
    if(subKey == "path")
      res = pathValue.maybeGetMember("path");

    else if(subKey == "value")
      res = pathValue.maybeGetMember("value");

    else
      res = pathValue;
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getValue",
      "",
      e.getDesc_cstr());
  }

  return res;
}

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
    FabricException::Throw(
      "BaseRTValScriptableCommand::isArg",
      "setting arg of '" + getName() + "', key not specified");

  if(!hasArg(key)) 
    // TODO: make this an optional behavior
    FabricException::Throw(
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
    FabricException::Throw(  
      "BaseRTValScriptableCommand::getArg",
      "No arg named '" + key + "' in command '" + getName() + "'");

  // Known RTVal of known type, get the json from it.
  return (m_rtvalArgs[key].second.isEmpty() && IsKnownRTValType(m_rtvalArgSpecs[key].type))
    ? RTValUtil::toJSON(getRTValArg(key))
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
    FabricException::Throw(
      "BaseRTValScriptableCommand::setArg",
      "No arg named " + mainKey + "' in command '" + getName() + "'");

  try
  {
    if( isPathValueArg(mainKey) && isJSONPathValueArg(json) )
    {
      RTVal rtVal = RTValUtil::fromJSON(
        FabricApplicationStates::GetAppStates()->getContext(),
        json,
        "PathValue");

      setRTValArg(mainKey, rtVal);
    }

    // Known type, cast the JSON to a RTVal.
    else if(IsKnownRTValType(m_rtvalArgSpecs[mainKey].type))
    {      
      RTVal rtVal = RTValUtil::fromJSON(
        FabricApplicationStates::GetAppStates()->getContext(),
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
    FabricException::Throw(
      "BaseRTValScriptableCommand::setArg",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
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
      FabricException::Throw(
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

void BaseRTValScriptableCommand::declareRTValArg(
  const QString &key, 
  const QString &type,
  int flags, 
  RTVal defaultValue) 
{
  if(key.isEmpty()) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::declareRTValArg",
      "Key not specified  in command '" + getName() + "'");
 
  if(!type.isEmpty() && !FabricApplicationStates::GetAppStates()->getClient().isValidType(type.toUtf8().constData()))
    FabricException::Throw(
      "BaseRTValScriptableCommand::declareRTValArg", 
      "Type '" + type  + "' of command '" + getName() + "' is not a valid KL type");

  try
  {
    ScriptableCommandRTValArgSpec spec;
    spec.type = type;
    spec.flags = flags;

    QString complexArgType;
    spec.defaultValue = IsPathValueArg(flags)
      ? RTVal::Construct(
          FabricApplicationStates::GetAppStates()->getContext(), 
          "PathValue", 0, 0)
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
    FabricException::Throw(
      "BaseRTValScriptableCommand::declareRTValArg",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
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
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArgType",
      "No arg named '" + mainKey + "' in command '" + getName() + "'");

  return isPathValueArg(mainKey)
    ? GetPathValueArgType(key, m_rtvalArgSpecs[mainKey].type)
    : m_rtvalArgSpecs[mainKey].type;
}

RTVal BaseRTValScriptableCommand::getRTValArg(
  const QString &key)
{
  QString mainKey = GetMainKey(key);

  if(!hasArg(mainKey)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "No arg named '" + mainKey + "' in command '" + getName() + "'");
  
  if(!m_rtvalArgs[mainKey].first.isValid() && !m_rtvalArgs[mainKey].second.isEmpty())
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
        "RTVal argument '" + mainKey + "' of command '" + getName() + 
        "' has been set in JSON only, \n !!! use getRTValArg(mainKey, type) instead !!!");

  RTVal val;

  try
  {
    val = isPathValueArg(mainKey)
      ? GetPathValueArg(key, RTValUtil::toRTVal(m_rtvalArgs[mainKey].first))
      : m_rtvalArgs[mainKey].first;

    val = RTValUtil::toRTVal(val);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
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
  QString mainKey = GetMainKey(key);

  if(!hasArg(mainKey)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "No arg named '" + mainKey + "' in command '" + getName() + "'");
 
  if(!FabricApplicationStates::GetAppStates()->getClient().isValidType(type.toUtf8().constData()))
    FabricException::Throw(
      "BaseDFGCommand::getRTValArg",
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
      RTVal val = RTValUtil::fromJSON(
        FabricApplicationStates::GetAppStates()->getContext(),
        m_rtvalArgs[mainKey].second,
        m_rtvalArgSpecs[mainKey].type);

      setRTValArg(key, val);
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "",
      e.what());
  }

  return getRTValArg(key);
}

void BaseRTValScriptableCommand::setRTValArg(
  const QString &key, 
  RTVal value) 
{ 
  QString mainKey = GetMainKey(key);

  if(!hasArg(mainKey)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::setRTValArg",
      "No arg named '" + mainKey + "' in command '" + getName() + "'");

  try
  {
    QPair<FabricCore::RTVal, QString> pair;

    // Sets the RTVal.
    if(isPathValueArg(mainKey))
    {
      RTVal pathValue = isArgSet(mainKey)
        ? RTValUtil::toRTVal( m_rtvalArgs[mainKey].first)
        : RTVal::Construct(value.getContext(), "PathValue", 0, 0);

      QString type = SetPathValueArg(key, value, pathValue);
      if(!type.isEmpty() && !IsKnownRTValType(m_rtvalArgSpecs[mainKey].type))
        m_rtvalArgSpecs[mainKey].type = type;

      pair.first = pathValue;
    }

    else
    {
      pair.first = value;

      if(!IsKnownRTValType(m_rtvalArgSpecs[mainKey].type))
        m_rtvalArgSpecs[mainKey].type = RTValUtil::getType(value);
    }

    m_rtvalArgs.insert(mainKey, pair);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::setRTValArg",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::setRTValArg",
      "",
      e.what());
  }
}

bool BaseRTValScriptableCommand::isPathValueArg(
  const QString &key)
{ 
  return IsPathValueArg(m_rtvalArgSpecs[key].flags);
}

bool BaseRTValScriptableCommand::isJSONPathValueArg(
  const QString &json)
{ 
  return json.indexOf("value") > -1
    && json.indexOf("path") > -1;
}

QString BaseRTValScriptableCommand::getPathValueArgPath(
  const QString &key)
{
  if(!hasArg(key)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::getPathValueArgPath",
      "No arg named '" + key + "' in command '" + getName() + "'");
  
  if(!isPathValueArg(key)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::getPathValueArgPath",
      "Arg named '" + key + "' in command '" + getName() + "' is not a PathValue");

  return getRTValArg(key+".path").getStringCString();
}

RTVal BaseRTValScriptableCommand::getPathValueArgValue(
  const QString &key)
{
  if(!hasArg(key)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::getPathValueArgValue",
      "No arg named '" + key + "' in command '" + getName() + "'");
  
  if(!isPathValueArg(key)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::getPathValueArgValue",
      "Arg named '" + key + "' in command '" + getName() + "' is not a PathValue");

  return getRTValArg(key+".value");
}

RTVal BaseRTValScriptableCommand::getPathValueArgValue(
  const QString &key,
  const QString &type)
{
  if(!hasArg(key)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::getPathValueArgValue",
      "No arg named '" + key + "' in command '" + getName() + "'");
  
  if(!isPathValueArg(key)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::getPathValueArgValue",
      "Arg named '" + key + "' in command '" + getName() + "' is not a PathValue");

  return getRTValArg(key+".value", type);
}

void BaseRTValScriptableCommand::setPathValueArgValue(
  const QString &key, 
  RTVal value) 
{ 
  if(!hasArg(key)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::setPathValueArgValue",
      "No arg named '" + key + "' in command '" + getName() + "'");

  if(!isPathValueArg(key)) 
    FabricException::Throw(
      "BaseRTValScriptableCommand::setPathValueArgValue",
      "Arg named '" + key + "' in command '" + getName() + "' is not a PathValue");

  setRTValArg(key+".value", value);
}
 