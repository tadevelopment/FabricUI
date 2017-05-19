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
    // If the value is an Object, we have use the RTValToJSONEncoder interface, if any.
    // This is the same logic as in FabricServices::Persistence::RTValToJSONEncoder, which unfortunately is not obvious to reuse.
    if( rtVal.isObject() ) {
      FabricCore::RTVal cast = FabricCore::RTVal::Construct( rtVal.getContext(), "RTValToJSONEncoder", 1, &rtVal );
      if( !cast.isValid() )
        throw FabricCore::Exception( ("KL object of type " + std::string(rtVal.getTypeNameCStr()) + " doesn't support RTValToJSONEncoder").c_str() );

      FTL::CStrRef ref;
      FabricCore::RTVal result;

      if( !cast.isNullObject() ) {
        result = cast.callMethod( "String", "convertToString", 0, 0 );
        if( !result.isValid() )
          throw FabricCore::Exception( ("Calling method 'RTValToJSONEncoder::convertToString' on object of type " + std::string( rtVal.getTypeNameCStr() ) + " failed").c_str() );

        ref = result.getStringCString();
      }
      // Encode as a json friendly string (eg: add escape chars)
      std::string json;
      {
        FTL::JSONEnc<> jsonEnc( json );
        FTL::JSONStringEnc<> jsonStringEnc( jsonEnc, ref );
      }
      res = json.c_str();
    } else
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
  Context context,
  const QString &json,
  const QString &rtValType)
{
  RTVal rtVal;
  
  try 
  {
    rtVal = RTVal::Construct(
      context,
      rtValType.toUtf8().constData(), 
      0, 
      0);

    // If the value is an Object, we have use the RTValToJSONDecoder interface, if any.
    // This is the same logic as in FabricServices::Persistence::RTValToJSONDecoder, which unfortunately is not obvious to reuse.
    if( rtVal.isObject() ) {

      // Create the object (non-null)
      rtVal = RTVal::Create(
        context,
        rtValType.toUtf8().constData(),
        0,
        0 );

      // Try to decode as a String first
      std::string decodedString;
      {
        FTL::StrRef jsonStr( json.toUtf8().constData() );
        FTL::JSONStrWithLoc strWithLoc( jsonStr );
        FTL::JSONDec<FTL::JSONStrWithLoc> jsonDec( strWithLoc );
        FTL::JSONEnt<FTL::JSONStrWithLoc> jsonEnt;
        if( !jsonDec.getNext( jsonEnt )
            || jsonEnt.getType() != jsonEnt.Type_String )
          return rtVal;// Return as empty
        jsonEnt.stringAppendTo( decodedString );
      }

      FabricCore::RTVal cast = FabricCore::RTVal::Construct( context, "RTValFromJSONDecoder", 1, &rtVal );
      if( !cast.isInterface() )
        return rtVal;// Return as empty
      if( cast.isNullObject() )
        return rtVal;// Return as empty

      FabricCore::RTVal data =
        FabricCore::RTVal::ConstructString(
          context,
          decodedString.data(),
          decodedString.size()
        );
      FabricCore::RTVal result = cast.callMethod( "Boolean", "convertFromString", 1, &data );
      if( !result.isValid() || !result.getBoolean() )
        throw FabricCore::Exception( ("Error calling 'RTValFromJSONDecoder::convertFromString' on object of type " + std::string( rtVal.getTypeNameCStr() ) ).c_str() );
    } else
      rtVal.setJSON( json.toUtf8().constData() );
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
  Context context,
  const QString &json,
  const QString &rtValType)
{
  RTVal rtVal = RTValUtil::jsonToRTVal(
    context,
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
