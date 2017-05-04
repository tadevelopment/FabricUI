//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandHelpers.h"
#include "KLCommandRegistry.h"
#include <FabricUI/Util/RTValUtil.h>
#include "BaseRTValScriptableCommand.h"

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

// ScriptableCommand
void BaseRTValScriptableCommand::declareArg(
  const QString &key, 
  bool optional, 
  const QString &defaultValue) 
{
  declareRTValArg(
    key,
    QString(),
    optional,
    defaultValue);
}

bool BaseRTValScriptableCommand::hasArg(
  const QString &key)
{
  return m_rtvalArgSpecs.count(key) > 0;
}

QList<QString> BaseRTValScriptableCommand::getArgKeys()
{
  return m_rtvalArgSpecs.keys(); 
}

QString BaseRTValScriptableCommand::getArg(
  const QString &key)
{
  if(!hasArg(key)) 
    printAndThrow(
      std::string(
        "BaseRTValScriptableCommand::getArg, error: no arg named " +
        std::string(key.toUtf8().constData())
      )
    );


  // Known RTVal of known type, get the json from it.
  if(m_rtvalArgs.count(key) > 0 && !m_rtvalArgSpecs[key].type.isEmpty())
  {
    QString json;
    try
    {
      json = Util::RTValUtil::rtValToJSON(
        getRTValArg(key));
    }
    catch(Exception &e)
    {
      json = Util::RTValUtil::klRTValToJSON(
        getRTValArg(key));
    }
    return json;
  }

  // Else, get the JSON directly
  else 
    return m_args.count(key) > 0 
      ? m_args[key]
      : QString();
}

inline RTVal castJSONToRTVal(
  const QString &json,
  const QString &type) 
{
  KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
    Commands::CommandRegistry::GetCommandRegistry());

  RTVal rtVal;

  try
  {
    rtVal = Util::RTValUtil::jsonToRTVal(
      registry->getClient(),
      json,
      type);
  }

  catch(Exception &e)
  {
    rtVal = Util::RTValUtil::jsonToKLRTVal(
      registry->getClient(),
      json,
      type);
  }

  if(!rtVal.isValid()) 
    printAndThrow(
      "BaseRTValScriptableCommand::castJSONToRTVal, error: wasn't able to cast JSON in RTVal"
    );

  return rtVal;
}

void BaseRTValScriptableCommand::setArg(
  const QString &key, 
  const QString &json) 
{
  if(!hasArg(key)) 
    printAndThrow(
      std::string(
        "BaseRTValScriptableCommand::setArg, error: no arg named " +
        std::string(key.toUtf8().constData())
      )
    );

  // Known type, cast the JSON to a RTVal.
  if(!m_rtvalArgSpecs[key].type.isEmpty())
  {
    RTVal rtVal = castJSONToRTVal(
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
  while (it.hasNext()) 
  {
    it.next();
    QString key = it.key();
    ScriptableCommandRTValArgSpec spec = it.value();
    
    bool isValid = !m_args.count(key) && m_args[key].isEmpty();
    bool isRTValValid = !m_rtvalArgs.count(key) && !m_rtvalArgs[key].isValid();

    // We support unknown type.
    if(!spec.optional && (!isValid && !isRTValValid))
      printAndThrow(
        std::string(
          "BaseRTValScriptableCommand::validateSetArgs, error: argument: '" + 
          std::string(key.toUtf8().constData()) + "' has not been set"
        )
      );
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
      + "] opt: "   + QString::number(spec.optional)
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
  bool optional, 
  const QString &defaultValue) 
{
  try
  {
    RTVal defaultRtVal;

    // Cast the JSON in RTVal of possible
    if(!type.isEmpty() && !defaultValue.isEmpty())
      defaultRtVal = castJSONToRTVal(
        getArg(key),
        type);

    declareRTValArg(
      key,
      type,
      optional,
      defaultRtVal);
  }

  catch(Exception &e)
  {
    printf(
      "BaseRTValScriptableCommand::declareRTValArg: exception: %s\n", 
      e.getDesc_cstr());
  }
}

void BaseRTValScriptableCommand::declareRTValArg(
  const QString &key, 
  const QString &type,
  bool optional, 
  RTVal defaultValue) 
{
  if(key.isEmpty()) 
    printAndThrow(
      "BaseRTValScriptableCommand::declareRTValArg, error: key not specified"
    );

  ScriptableCommandRTValArgSpec spec;
  spec.type = type;
  spec.optional = optional;
  m_rtvalArgSpecs.insert(key, spec);

  if(defaultValue.isValid())
  {
    m_rtvalArgSpecs[key].defaultValue = defaultValue;
    setRTValArg(key, defaultValue);
  }
}
 
QString BaseRTValScriptableCommand::getRTValArgType(
  const QString &key)
{
  if(!hasArg(key)) 
    printAndThrow(
      std::string(
        "BaseRTValScriptableCommand::getRTValArgType, error: no arg named " +
        std::string(key.toUtf8().constData())
      )
    );

  return m_rtvalArgSpecs[key].type;
}

RTVal BaseRTValScriptableCommand::getRTValArg(
  const QString &key)
{
  RTVal rtVal;

  if(!hasArg(key)) 
    printAndThrow(
      std::string(
        "BaseRTValScriptableCommand::getRTValArg, error: no arg named " +
        std::string(key.toUtf8().constData())
      )
    );

  if(m_rtvalArgs.count(key) == 0)
  {
    printAndThrow(
      std::string(
        "BaseRTValScriptableCommand::getRTValArg, error: no arg named " +
        std::string(key.toUtf8().constData())
      )
    );
  }

  if(m_rtvalArgs[key].isValid())
  {
    try
    {
      rtVal = Util::RTValUtil::klRTValToRTVal(
        m_rtvalArgs[key]);
    }
    catch(Exception &e)
    {
      rtVal = m_rtvalArgs[key];
    }
  }

  return rtVal;
}

RTVal BaseRTValScriptableCommand::getRTValArg(
  const QString &key,
  const QString &type)
{
  if(!hasArg(key)) 
    printAndThrow(
      std::string(
        "BaseRTValScriptableCommand::getRTValArg, error: no arg named " +
        std::string(key.toUtf8().constData())
      )
    );

  RTVal rtVal;

  // The RTVal is set already, get it.
  if(m_rtvalArgs.count(key) > 0)
    rtVal = getRTValArg(key);

  // The RTVal's been set in pure JSON -> construct it.
  else
  {
    rtVal = castJSONToRTVal(
      getArg(key),
      type);
   
    setRTValArg(
      key, 
      rtVal);
  }

  return rtVal;
}

void BaseRTValScriptableCommand::setRTValArg(
  const QString &key, 
  RTVal value) 
{
  if(!hasArg(key)) 
    printAndThrow(
      std::string(
        "BaseRTValScriptableCommand::setRTValArg, error: no arg named " +
        std::string(key.toUtf8().constData())
      )
    );

  // Sets the RTVal.
  m_rtvalArgs.insert(key, value);

  // Get its type.
  try
  {
    if(m_rtvalArgSpecs[key].type.isEmpty())
      m_rtvalArgSpecs[key].type = value.callMethod(
        "String",
        "type",
        0,
        0).getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "BaseRTValScriptableCommand::setRTValArg: exception: %s\n", 
      e.getDesc_cstr());
  }
}
