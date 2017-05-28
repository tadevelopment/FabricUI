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
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace Application;

bool RTValUtil::isKLRTVal(
  RTVal klRTVal)
{
  return klRTVal.isWrappedRTVal();
}

QString RTValUtil::getType(
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
    FabricException::Throw(
      "RTValUtil::getType",
      "",
      e.getDesc_cstr());
  }

  return type;
}

RTVal RTValUtil::toRTVal(
  RTVal klRTVal)
{
  RTVal rtVal;

  try 
  {
    rtVal = isKLRTVal(klRTVal)
      ? klRTVal.getUnwrappedRTVal()
      : klRTVal;
   }

  catch(Exception &e)
  {
    FabricException::Throw(
      "RTValUtil::toKLRTVal",
      "",
      e.getDesc_cstr());
  }

  return rtVal;
}

RTVal RTValUtil::toKLRTVal(
  RTVal rtVal)
{
  RTVal klRTVal;

  try 
  {
    klRTVal = !isKLRTVal(rtVal)
      ? RTVal::ConstructWrappedRTVal(rtVal)
      : rtVal;
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "RTValUtil::toKLRTVal",
      "",
      e.getDesc_cstr());
  }

  return klRTVal;
}

QString RTValUtil::toJSON(
  RTVal rtVal_)
{
  QString res;

  try 
  {
    RTVal rtVal = toRTVal(rtVal_);

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
    FabricException::Throw(
      "RTValUtil::toJSON",
      "",
      e.getDesc_cstr());
  }

  catch(FTL::JSONException &je)
  {
    FabricException::Throw(
      "RTValUtil::toJSON",
      "Caught JSONException",
      je.getDescCStr());
  }

  return res;
}

RTVal RTValUtil::fromJSON(
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
    FabricException::Throw(
      "RTValUtil::fromJSON",
      "",
      e.getDesc_cstr());
  }

  catch(FTL::JSONException &je)
  {
    FabricException::Throw(
      "RTValUtil::fromJSON",
      "Caught JSONException",
      je.getDescCStr());
  }

  return rtVal;
}

RTVal RTValUtil::fromJSON(
  Client client,
  const QString &json,
  const QString &rtValType)
{
  return fromJSON(
    client.getContext(),
    json,
    rtValType);
}
