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
    RTValUtil provides static methods to convert 
    C++ KL RTVal to/from JSON. A C++ KL RTVal is
    a C++ RTVal wrapping a KL RTVal.
  */

  public:
    RTValUtil();

    /// Convert a C++ KL RTVal to a C++ RTVal.
    static FabricCore::RTVal klRTValToRTVal(
      FabricCore::RTVal klRTVal
      );

    /// Convert a C++ RTVal to a C++ KL RTVal.
    static FabricCore::RTVal rtValToKLRTVal(
      FabricCore::RTVal rtVal
      );

    /// Extract in JSON the C++ RTVal.
    static QString rtValToJSON(
      FabricCore::RTVal rtVal
      );

    /// Extract in JSON the C++ KL RTVal.
    static QString klRTValToJSON(
      FabricCore::RTVal klRTVal
      );

    /// Create a C++ RTVal of type rtValType 
    /// and sets it's value from the json.
    static FabricCore::RTVal jsonToRTVal(
      FabricCore::Context ctxt,
      const QString &json,
      const QString &rtValType
      );
     
    /// Create a C++ RTVal wrapping a KL RTVal 
    /// of type rtValType and sets it's value
    /// from the json.
    static FabricCore::RTVal jsonToKLRTVal(
      FabricCore::Context ctxt,
      const QString &json,
      const QString &rtValType
      );

    /// Create a C++ RTVal of type rtValType 
    /// and sets it's value from the json.
    static FabricCore::RTVal jsonToRTVal(
      FabricCore::Client client,
      const QString &json,
      const QString &rtValType
      );

    /// Create a C++ RTVal wrapping a KL RTVal 
    /// of type rtValType and sets it's value
    /// from the json.
    static FabricCore::RTVal jsonToKLRTVal(
      FabricCore::Client client,
      const QString &json,
      const QString &rtValType
      );

    /// Convert a string in
    /// a scriptable format.
    static QString EncodeString(
      QString str
      );

    /// Convert a list of strings 
    /// in a scriptable format.
    static QString EncodeStrings(
      QList< QString > strList
      );
};

} // namespace FabricUI
} // namespace Util

#endif // __UI_RTVAL_UTIL__
