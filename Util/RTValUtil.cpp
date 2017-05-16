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
    // Not ncesseray to parse the JSON.
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
