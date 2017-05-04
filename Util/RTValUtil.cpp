//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
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
      klRTVal.getContext(), 
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
  RTVal rtVal)
{
  RTVal klRTVal;

  try 
  {
    klRTVal = RTVal::Construct(
      rtVal.getContext(), 
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

QString RTValUtil::klRTValToJSON(
  RTVal klRTVal)
{
  return RTValUtil::rtValToJSON(
    klRTValToRTVal(
      klRTVal)
    );
}

RTVal RTValUtil::jsonToRTVal(
  Context ctxt,
  const QString &json,
  const QString &rtValType)
{
  RTVal rtVal;
  
  try 
  {
    rtVal = RTVal::Construct(
      ctxt, 
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

RTVal RTValUtil::jsonToKLRTVal(
  Context ctxt,
  const QString &json,
  const QString &rtValType)
{
  RTVal rtVal = RTValUtil::jsonToRTVal(
    ctxt,
    json,
    rtValType);
  
  return RTValUtil::rtValToKLRTVal(
    rtVal);
}

RTVal RTValUtil::jsonToRTVal(
  Client client,
  const QString &json,
  const QString &rtValType)
{
  return jsonToRTVal(
    client.getContext(),
    json,
    rtValType);
}

RTVal RTValUtil::jsonToKLRTVal(
  Client client,
  const QString &json,
  const QString &rtValType)
{
  return jsonToKLRTVal(
    client.getContext(),
    json,
    rtValType);
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
    // Don't pollute with newlines, ...
    // Not necesseray to parse the JSON.
    else if ( *it == '\r' ) result += "";//\\r";
    else if ( *it == '\n' ) result += "";//"\\n";
    else if ( *it == '\t' ) result += "";//"\\t";
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
  
  QString str;
  int count = 0;
  
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
