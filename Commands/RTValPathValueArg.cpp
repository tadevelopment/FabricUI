//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <QStringList>
#include "CommandArgFlags.h"
#include "CommandException.h"
#include "RTValPathValueArg.h"
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;

RTValPathValueArg::RTValPathValueArg() {}

RTValPathValueArg::~RTValPathValueArg() {}

inline QString ParseRTValPathValueArgKey(
  QString &mainKey)
{ 
  if(mainKey.indexOf(".") > -1) 
  {
    QList<QString> temp = mainKey.split(".");
    mainKey = temp[0];
    return temp[1];
  }
  return "";
}

QString RTValPathValueArg::getType()
{
  return "PathValue";
}

bool RTValPathValueArg::hasFlags(
  int flags)
{
  return (flags & CommandArgFlags::IN_ARG) || 
    (flags & CommandArgFlags::OUT_ARG) || 
    (flags & CommandArgFlags::IO_ARG);
}

bool RTValPathValueArg::isTypeOf(
  const QString &json)
{
  return json.indexOf("value") > -1 && 
    json.indexOf("path") > -1;
}

QString RTValPathValueArg::setValue(
  const QString &key, 
  RTVal value, 
  RTVal &pathValue) 
{
  QString type;

  try
  {
    QString mainKey = key;
    QString subKey = ParseRTValPathValueArgKey(mainKey);

    if(subKey == "path")
      pathValue.setMember("path", value);
    
    else if(subKey == "value" || RTValUtil::getRTValType(value) != "PathValue")
    {
      pathValue.setMember("value", value);
      type = RTValUtil::getRTValType(value);
    }

    else
    {
      pathValue = value;
      type = RTValUtil::getRTValType(
        RTValUtil::forceToRTVal(value).maybeGetMember("value"));
    }
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "RTValPathValueArg::setValue",
      "",
      e.getDesc_cstr());
  }

  return type;
}

QString RTValPathValueArg::getValueType(
  const QString &key, 
  const QString &type) 
{
  QString res;

  try
  {
    QString mainKey = key;
    QString subKey = ParseRTValPathValueArgKey(mainKey);
   
    if(subKey == "path")
      res = "String";

    else if(subKey != "value")
      res = "PathValue";

    else
      res = type;
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "RTValPathValueArg::getValueType",
      "",
      e.getDesc_cstr());
  }

  return res;
}

RTVal RTValPathValueArg::getValue(
 const QString &key,
  RTVal pathValue)
{
  RTVal res;

  try
  {
    QString mainKey = key;
    QString subKey = ParseRTValPathValueArgKey(mainKey);
        
    if(subKey == "path")
      res = pathValue.maybeGetMember("path");

    else if(subKey == "value")
      res = pathValue.maybeGetMember("value");

    else
      res = pathValue;
  }

  catch(Exception &e)
  {
    CommandException::Throw(
      "RTValPathValueArg::getValue",
      "",
      e.getDesc_cstr());
  }

  return res;
}
