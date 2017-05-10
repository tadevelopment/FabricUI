//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_UTIL__
#define __UI_RTVAL_UTIL__

#include <QList>
#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Util {

class RTValUtil 
{
  /**
    RTValUtil provides static methods to convert C++ KL RTVal to/from JSON. 
    A C++ KL RTVal is a C++ RTVal wrapping a KL RTVal.
  */

  public:
    RTValUtil();

    /// Checks if the c++ RTVal wraps a KL RTVal.
    static bool isKLRTVal(
      FabricCore::RTVal klRTVal
      );

    /// Gets the KL type of the C++ RTVal.
    /// If the C++ RTVal wraps a KL RTVal, the returned type 
    /// is the type of the object wrapped by the KL RTVal.
    /// To know if a C++ wraps a KL RTVal, use `isKLRTVal`.
    static QString getRTValType(
      FabricCore::RTVal rtVal
      );

    /// Converts a C++ KL RTVal to a C++ RTVal.
    static FabricCore::RTVal klRTValToRTVal(
      FabricCore::RTVal klRTVal
      );

    /// Converts a C++ RTVal to a C++ KL RTVal.
    static FabricCore::RTVal rtValToKLRTVal(
      FabricCore::RTVal rtVal
      );

    /// Extracts in JSON the C++ RTVal.
    static QString rtValToJSON(
      FabricCore::RTVal rtVal
      );

    /// Extracts in JSON the C++ KL RTVal.
    static QString klRTValToJSON(
      FabricCore::RTVal klRTVal
      );

    /// Creates a C++ RTVal of KL type 'type' 
    /// and sets it's value from the json.
    static FabricCore::RTVal jsonToRTVal(
      FabricCore::Context ctxt,
      const QString &json,
      const QString &type
      );
     
    /// Creates a C++ RTVal wrapping a KL RTVal of
    /// KL type 'type' and sets it's value from json.
    static FabricCore::RTVal jsonToKLRTVal(
      FabricCore::Context ctxt,
      const QString &json,
      const QString &type
      );

    /// Creates a C++ RTVal of KL type 'type' 
    /// and sets it's value from the json.
    static FabricCore::RTVal jsonToRTVal(
      FabricCore::Client client,
      const QString &json,
      const QString &type
      );

    /// Creates a C++ RTVal wrapping a KL RTVal of
    /// KL type 'type' and sets it's value from json.
    static FabricCore::RTVal jsonToKLRTVal(
      FabricCore::Client client,
      const QString &json,
      const QString &type
      );

    /// Creates a C++ RTVal wrapping a KL RTVal of
    /// KL type 'type' and sets it's value from json.
    static FabricCore::RTVal forceJSONToRTVal(
      FabricCore::Context ctxt,
      const QString &json,
      const QString &type);

    /// Creates a C++ RTVal wrapping a KL RTVal of
    /// KL type "type" and sets it's value from json.
    static FabricCore::RTVal forceJSONToRTVal(
      FabricCore::Client client,
      const QString &json,
      const QString &type);

    /// Converts a C++ KL RTVal to a C++ RTVal
    /// if the input is a C++ KL RTVal.
    static FabricCore::RTVal forceToRTVal(
      FabricCore::RTVal rtVal);

    /// Converts a C++ RTVal to a C++ KL 
    /// RTVal if the input is a C++ RTVal.
    static FabricCore::RTVal forceToKLRTVal(
      FabricCore::RTVal rtVal);

    /// Extracts the JSON from the C++ RTVal 
    /// either it wraps a KL RTVal or others.
    static QString forceRTValToJSON(
      FabricCore::RTVal rtVal);
};

} // namespace FabricUI
} // namespace Util

#endif // __UI_RTVAL_UTIL__
