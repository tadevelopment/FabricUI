//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <QRegExp>
#include <iostream>
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

bool RTValUtil::isKLRTVal(
  RTVal klRTVal)
{
  QString dataType = 
    klRTVal.getTypeName().getStringCString();
  return dataType == "RTVal";
}

QString RTValUtil::getRTValType(
  RTVal klRTVal)
{
  QString type;

  try 
  {
    type = klRTVal.callMethod(
      "String", 
      "type", 
      0, 
      0).getStringCString();
  }

  catch(Exception &e)
  {
    printf(
      "RTValUtil::getRTValType: exception: %s\n", 
      e.getDesc_cstr());
  }

  return type;
}

RTVal RTValUtil::klRTValToRTVal(
  RTVal klRTVal)
{
  RTVal rtVal;

  try 
  {
    // Get the type of the value  
    // stored by the KL RTVal
    const char *dataType = klRTVal.callMethod(
      "String", 
      "type", 
      0, 
      0).getStringCString();
    
    // Construct a C++ RTVal of type `type`
    // and set its value from the kl RTVal.
    rtVal = RTVal::Construct(
      klRTVal.getContext(), 
      dataType, 
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

RTVal RTValUtil::forceJSONToRTVal(
  Context ctxt,
  const QString &json,
  const QString &type) 
{
  RTVal rtVal;

  try
  {
    rtVal = Util::RTValUtil::jsonToRTVal(
      ctxt,
      json,
      type);
  }

  catch(Exception &e)
  {
    rtVal = Util::RTValUtil::jsonToKLRTVal(
      ctxt,
      json,
      type);
  }

  if(!rtVal.isValid()) 
    printf(
      "RTValUtil::forceJSONToRTVal, error: wasn't able to cast JSON in RTVal"
    );

  return rtVal;
}

RTVal RTValUtil::forceJSONToRTVal(
  Client client,
  const QString &json,
  const QString &type) 
{
  return forceJSONToRTVal(
    client.getContext(),
    json,
    type);
}

RTVal RTValUtil::forceToRTVal(
  RTVal rtVal) 
{
  return isKLRTVal(rtVal)
    ? klRTValToRTVal(rtVal)
    : rtVal;
}

RTVal RTValUtil::forceToKLRTVal(
  RTVal rtVal) 
{
  return !isKLRTVal(rtVal) 
    ? rtValToKLRTVal(rtVal)
    : rtVal;
}

QString RTValUtil::forceRTValToJSON(
  RTVal rtVal) 
{
  return isKLRTVal(rtVal)
    ? RTValUtil::klRTValToJSON(rtVal)
    : RTValUtil::rtValToJSON(rtVal);
}
