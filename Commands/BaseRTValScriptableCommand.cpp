//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <iostream>
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
 
bool BaseRTValScriptableCommand::isArgTypeKnown(
  const QString &key) 
{
  return m_rtvalArgSpecs[key].type != "RTVal";
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

void BaseRTValScriptableCommand::checkHasArg(
  const QString &methodName,
  const QString &key)
{
  if(!hasArg(key)) 
    FabricException::Throw(
      methodName,
      "No arg named '" + key + "' in command '" + getName() + "'");
}

void BaseRTValScriptableCommand::checkEmptyKey(
  const QString &methodName,
  const QString &key)
{
  if(key.isEmpty()) 
    FabricException::Throw(
      methodName,
      "Command '" + getName() + "', key not specified");
}

void BaseRTValScriptableCommand::checkRTValType(
  const QString &methodName,
  const QString &key,
  const QString &type)
{
  if(!FabricApplicationStates::GetAppStates()->getClient().isValidType(type.toUtf8().constData()))
    FabricException::Throw(
      methodName,
      "Argument '" + key + "' in command '" + getName() + "' has not a valid kl type '" + type + "'");
}

bool BaseRTValScriptableCommand::isArg(
  const QString &key,
  int flag)
{
  checkEmptyKey("BaseRTValScriptableCommand::isArg", key); 
  checkHasArg("BaseRTValScriptableCommand::isArg", key); 
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
    ( RTValUtil::getType(getRTValArgValue(key)) == m_rtvalArgSpecs[key].type ||
      !getRTValArgPath(key).isEmpty() || !m_rtvalArgs[key].second.isEmpty() 
    );
}

QString BaseRTValScriptableCommand::getArg(
  const QString &key)
{
  checkHasArg("BaseRTValScriptableCommand::getArg", key); 

  // Known RTVal of known type, get the json from it.
  return (m_rtvalArgs[key].second.isEmpty() && isArgTypeKnown(key))
    ? RTValUtil::toJSON(getRTValArg(key))
    // Otherwise, return the Json if it's been set.
    // It happens if the arg's been declared with an unknown type
    : m_rtvalArgs[key].second;
}

void BaseRTValScriptableCommand::setArg(
  const QString &key, 
  const QString &json) 
{
  checkHasArg("BaseRTValScriptableCommand::setArg", key); 

  std::cout 
    << "BaseRTValScriptableCommand::setArg 1 "
    << key.toUtf8().constData()
    << " "
    << m_rtvalArgSpecs[key].type.toUtf8().constData()
    << " "
    << json.toUtf8().constData()
    << std::endl;
  try
  {
    if( isJSONPathValueArg(json) )
    {
      RTVal pathValue = RTValUtil::fromJSON(
        FabricApplicationStates::GetAppStates()->getContext(),
        json,
        "PathValue");
      
      setRTValArg(key, pathValue);
    }

    // Known type, cast the JSON to a RTVal.
    else if(isArgTypeKnown(key))
    {      
      RTVal rtVal = RTValUtil::fromJSON(
        FabricApplicationStates::GetAppStates()->getContext(),
        json,
        m_rtvalArgSpecs[key].type);

      setRTValArgValue(key, rtVal);
    }

    // Store the JSON directly.
    else
      m_rtvalArgs[key].second = json;

    std::cout 
      << "BaseRTValScriptableCommand::setArg 2 "
      << key.toUtf8().constData()
      << " "
      << json.toUtf8().constData()
      << std::endl;
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

    // We support unknown type.
    if(!isArg(key, CommandArgFlags::OPTIONAL_ARG) && !isArgSet(key))
      FabricException::Throw(
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
  checkEmptyKey("BaseRTValScriptableCommand::declareRTValArg", key); 
  checkRTValType("BaseRTValScriptableCommand::declareRTValArg", key, type); 
 
  try
  {
    ScriptableCommandRTValArgSpec spec;
    spec.type = type;
    spec.flags = flags;
    spec.defaultValue = defaultValue;
    m_rtvalArgSpecs.insert(key, spec);

    RTVal pathValue;
    Context context = FabricApplicationStates::GetAppStates()->getContext();
    
    if(!type.isEmpty() && defaultValue.isValid())
    {
      RTVal args[2] = {
        RTVal::ConstructString(context, ""),
        RTValUtil::toKLRTVal(defaultValue)
      };

      pathValue = RTVal::Construct(
        context, 
        "PathValue", 
        2, 
        args);
    }
    else
      pathValue = RTVal::Construct(
        context, 
        "PathValue", 
        0, 
        0);
   
    QPair<FabricCore::RTVal, QString> pair;
    pair.first = pathValue;
    m_rtvalArgs.insert(key, pair);
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
  checkHasArg("BaseRTValScriptableCommand::getRTValArgType", key); 
  return m_rtvalArgSpecs[key].type;
}

QString BaseRTValScriptableCommand::getRTValArgPath(
  const QString &key)
{
  checkHasArg("BaseRTValScriptableCommand::getRTValArgPath", key); 
   
  QString type;

  try
  { 
    type = RTValUtil::toRTVal(
      RTValUtil::toRTVal(m_rtvalArgs[key].first).maybeGetMember("path")
      ).getStringCString();
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArgPath",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArgPath",
      "",
      e.what());
  }

  return type;
}

RTVal BaseRTValScriptableCommand::getRTValArgValue(
  const QString &key)
{
  checkHasArg("BaseRTValScriptableCommand::getRTValArgValue", key); 
  
  if(!m_rtvalArgs[key].first.isValid() && !m_rtvalArgs[key].second.isEmpty())
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArgValue",
        "RTVal argument '" + key + "' of command '" + getName() + 
        "' has been set in JSON only, \n !!! use getRTValArgValue(key, type) instead !!!");

  RTVal val;

  try
  { 
    val = RTValUtil::toRTVal(
      RTValUtil::toRTVal(m_rtvalArgs[key].first).maybeGetMember("value")
      );
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArgValue",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArgValue",
      "",
      e.what());
  }

  return val;
}

RTVal BaseRTValScriptableCommand::getRTValArgValue(
  const QString &key,
  const QString &type)
{
  checkHasArg("BaseRTValScriptableCommand::getRTValArgValue", key); 
  checkRTValType("BaseRTValScriptableCommand::getRTValArgValue", key, type); 

  try
  {
    // If the type is unknown, set it
    //if(!isArgTypeKnown(key))
      m_rtvalArgSpecs[key].type = type;
 
    // If the arg has been set in JSON,  
    // construct it since we know its type
    if(!m_rtvalArgs[key].second.isEmpty())
    {
      RTVal val = RTValUtil::fromJSON(
        FabricApplicationStates::GetAppStates()->getContext(),
        m_rtvalArgs[key].second,
        m_rtvalArgSpecs[key].type);

      setRTValArgValue(key, val);
    }
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArgValue",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArgValue",
      "",
      e.what());
  }

  return getRTValArgValue(key);
}

void BaseRTValScriptableCommand::setRTValArg(
  const QString &key, 
  RTVal pathValue) 
{ 
  checkHasArg("BaseRTValScriptableCommand::setRTValArg", key); 

  if(RTValUtil::getType(pathValue) != "PathValue")
     FabricException::Throw(
      "BaseRTValScriptableCommand::setRTValArg",
      "Arg '" + key + "' is not a PathValue");

  try
  {
    QString type = RTValUtil::getType(
      RTValUtil::toRTVal(pathValue).maybeGetMember("value")
      );

    if(!type.isEmpty() && !isArgTypeKnown(key))
      m_rtvalArgSpecs[key].type = type;

    QPair<FabricCore::RTVal, QString> pair;
    pair.first = pathValue;
    m_rtvalArgs.insert(key, pair);
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

RTVal BaseRTValScriptableCommand::getRTValArg(
  const QString &key)
{
  checkHasArg("BaseRTValScriptableCommand::getRTValArg", key); 
 
  RTVal pathValue;

  try
  { 
    pathValue =/* isArgSet(key)*/
      /*?*/ RTValUtil::toRTVal(m_rtvalArgs[key].first)
      /*: RTVal::Construct(
          FabricApplicationStates::GetAppStates()->getContext(), 
          "PathValue", 0, 0
          )*/;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::getRTValArg",
      "",
      e.what());
  }

  return pathValue;
}

void BaseRTValScriptableCommand::setRTValArgValue(
  const QString &key, 
  RTVal value) 
{ 
  checkHasArg("BaseRTValScriptableCommand::setRTValArgValue", key); 

  try
  {
    RTVal pathValue = getRTValArg(key);
    pathValue.setMember("value", RTValUtil::toKLRTVal(value));
    setRTValArg(key, pathValue);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::setRTValArgValue",
      "",
      e.getDesc_cstr());
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseRTValScriptableCommand::setRTValArgValue",
      "",
      e.what());
  }
}

bool BaseRTValScriptableCommand::isJSONPathValueArg(
  const QString &json)
{ 
  return json.indexOf("value") > -1 && json.indexOf("path") > -1;
}
