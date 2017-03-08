//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <iostream>
#include <QRegExp>
#include "RTValUtil.h"
#include <FTL/StrRef.h>
#include <FTL/JSONEnc.h>
#include <FTL/JSONDec.h>
#include <FTL/JSONValue.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;

RTValUtil::RTValUtil()
{
}

RTVal RTValUtil::klRTValToRTVal(
  Client client,
  RTVal klRTVal)
{
  RTVal rtVal;

  try 
  {
    // Get the type of the value  
    // stored by the KL RTVal
    QString dataType = klRTVal.callMethod(
      "String", 
      "type", 
      0, 
      0).getStringCString();
    
    // Construct a C++ RTVal of type `type`
    // and set its value from the kl RTVal.
    rtVal = RTVal::Construct(
      client, 
      dataType.toUtf8().constData(), 
      1, 
      &klRTVal);
  }

  catch(Exception &e)
  {
    printf(
      "RTValUtil::klRTValToRTVal: exception: %s\n", 
      e.getDesc_cstr());
  }

  return rtVal;
}

RTVal RTValUtil::rtValToKLRTVal(
  Client client,
  RTVal rtVal)
{
  RTVal klRTVal;

  try 
  {
    klRTVal = RTVal::Construct(
      client, 
      "RTVal", 
      1, 
      &rtVal);
  }

  catch(Exception &e)
  {
    printf(
      "RTValUtil::rtValToKLRTVal: exception: %s\n", 
      e.getDesc_cstr());
  }

  return klRTVal;
}

QString RTValUtil::rtValToJSON(
  Client client,
  RTVal rtVal)
{
  QString res;

  try 
  {
    res = rtVal.getJSON().getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "RTValUtil::rtValToJSON: exception: %s\n", 
      e.getDesc_cstr());
  }

  catch ( FTL::JSONException &je )
  {
    printf(
      "RTValUtil::rtValToJSON : Caught JSONException: %s\n", 
      je.getDescCStr() );
  } 

  return res;
}

RTVal RTValUtil::jsonToRTVal(
  Client client,
  const QString &json,
  const QString &rtValType)
{
  RTVal rtVal;
  
  try 
  {
    rtVal = RTVal::Construct(
      client, 
      rtValType.toUtf8().constData(), 
      0, 
      0);

    rtVal.setJSON(json.toUtf8().constData());
  }

  catch(Exception &e)
  {
    printf(
      "RTValUtil::jsonToRTVal: exception: %s\n", 
      e.getDesc_cstr());
  }

  catch ( FTL::JSONException &je )
  {
    printf(
      "RTValUtil::jsonToRTVal : Caught JSONException: %s\n", 
      je.getDescCStr() );
  } 

  return rtVal;
}

QString RTValUtil::klRTValToJSON(
  Client client,
  RTVal klRTVal)
{
  return RTValUtil::rtValToJSON(
    client,
    klRTValToRTVal(
      client,
      klRTVal)
    );
}

RTVal RTValUtil::jsonToKLRTVal(
  Client client,
  const QString &json,
  const QString &rtValType)
{
  RTVal rtVal = RTValUtil::jsonToRTVal(
    client,
    json,
    rtValType);
  
  return RTValUtil::rtValToKLRTVal(
    client, 
    rtVal);
}

inline QString EncodeStringChars(
  QString str)
{
  QString result;
 
  for(QString::const_iterator it(str.begin()); 
      it != str.end(); 
      ++it)
  {
    if ( *it == '\"' ) result += "\\\"";
    else if ( *it == '\r' ) result += "\\r";
    else if ( *it == '\n' ) result += "\\n";
    else if ( *it == '\t' ) result += "\\t";
    else if ( *it == '\\' ) result += "\\\\";
    else result += *it;
  }

  return result;
}

QString RTValUtil::EncodeString(
  QString str)
{
  return "\"" + EncodeStringChars(str) + "\"";
}

QString RTValUtil::EncodeStrings(
  QList< QString > strList)
{
  QString result;
  
  int count = 0;
  QString str;
  foreach(str, strList)
  {
    if(count > 0)
      result += "|";
    result += EncodeString(str);
    count ++;
  }
  result += "\"";

  return result;
}
