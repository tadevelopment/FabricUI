//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_UTIL__
#define __UI_RTVAL_UTIL__

#include <QString>
#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <FTL/JSONEnc.h>
#include <FTL/JSONDec.h>
#include <FTL/JSONValue.h>

namespace FabricUI {
namespace Util {

class RTValUtil 
{
  /**
    RTValUtil provides static methods to convert 
    C++ KL RTVal to/from JSON. A C++ KL RTVal is
    a C++ RTVal wrapping a KL RTVal.
  */

  public:
    RTValUtil()
    {
    }

    /// Convert a C++ KL RTVal to a C++ RTVal.
    static FabricCore::RTVal klRTValToRTVal(
      FabricCore::Client client,
      FabricCore::RTVal klRTVal)
    {
      FabricCore::RTVal rtVal;

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
        rtVal = FabricCore::RTVal::Construct(
          client, 
          dataType.toUtf8().constData(), 
          1, 
          &klRTVal);
      }

      catch(FabricCore::Exception &e)
      {
        printf(
          "RTValUtil::klRTValToRTVal: exception: %s\n", 
          e.getDesc_cstr());
      }

      return rtVal;
    }

    /// Convert a C++ RTVal to a C++ KL RTVal.
    static FabricCore::RTVal rtValToKLRTVal(
      FabricCore::Client client,
      FabricCore::RTVal rtVal)
    {
      FabricCore::RTVal klRTVal;

      try 
      {
        klRTVal = FabricCore::RTVal::Construct(
          client, 
          "RTVal", 
          1, 
          &rtVal);
      }

      catch(FabricCore::Exception &e)
      {
        printf(
          "RTValUtil::rtValToKLRTVal: exception: %s\n", 
          e.getDesc_cstr());
      }

      return klRTVal;
    }

    /// Extract in JSON the C++ RTVal.
    static QString rtValToJSON(
      FabricCore::Client client,
      FabricCore::RTVal rtVal)
    {
      QString res;

      try 
      {
        res = rtVal.getJSON().getStringCString();
      }

      catch(FabricCore::Exception &e)
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

    /// Create a C++ RTVal of type rtValType 
    /// and sets it's value from the json.
    static FabricCore::RTVal jsonToRTVal(
      FabricCore::Client client,
      const QString &json,
      const QString &rtValType)
    {
      FabricCore::RTVal rtVal;
      
      try 
      {
        rtVal = FabricCore::RTVal::Construct(
          client, 
          rtValType.toUtf8().constData(), 
          0, 
          0);

        rtVal.setJSON(json.toUtf8().constData());
      }

      catch(FabricCore::Exception &e)
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

    /// Extract in JSON the C++ KL RTVal.
    static QString klRTValToJSON(
      FabricCore::Client client,
      FabricCore::RTVal klRTVal)
    {
      return RTValUtil::rtValToJSON(
        client,
        klRTValToRTVal(
          client,
          klRTVal)
        );
    }
     
    /// Create a C++ RTVal wrapping a KL RTVal 
    /// of type rtValType and sets it's value
    /// from the json.
    static FabricCore::RTVal jsonToKLRTVal(
      FabricCore::Client client,
      const QString &json,
      const QString &rtValType)
    {
      FabricCore::RTVal rtVal = RTValUtil::jsonToRTVal(
        client,
        json,
        rtValType);
      
      return RTValUtil::rtValToKLRTVal(
        client, 
        rtVal);
    }
};

} // namespace FabricUI
} // namespace Util

#endif // __UI_RTVAL_UTIL__
