//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_PATH_VALUE_ARG__
#define __UI_PATH_VALUE_ARG__

#include "RTValComplexArg.h"

namespace FabricUI {
namespace Commands {

class RTValPathValueArg : public RTValComplexArg {
  /**
    RTValPathValueArg specializes RTValComplexArg for KL PathValue.
    PathValue are used to automatically resolves the RTVal from a path.

    They can be used to get/set the DFG ports or variables and or any
    KL object/structur which can be retreived from a path.
  */

  public:
    RTValPathValueArg();

    virtual ~RTValPathValueArg();

    /// Implementation of RTValComplexArg.
    virtual QString getType();

    /// Implementation of RTValComplexArg.
    virtual bool hasFlags(
      int flags
      );

    /// Implementation of RTValComplexArg.
    virtual bool isTypeOf(
      const QString &json
      );

    /// Implementation of RTValComplexArg.
    virtual QString setValue(
      const QString &key, 
      FabricCore::RTVal value, 
      FabricCore::RTVal &pathValue
      );

    /// Implementation of RTValComplexArg.
    virtual QString getValueType(
      const QString &key, 
      const QString &type
      );

    /// Implementation of RTValComplexArg.
    virtual FabricCore::RTVal getValue(
      const QString &key,
      FabricCore::RTVal pathValue
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_PATH_VALUE_ARG__
