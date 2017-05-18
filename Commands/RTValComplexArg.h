//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMPLEX_ARG__
#define __UI_COMPLEX_ARG__

#include <QString>
#include <FabricCore.h>

namespace FabricUI {
namespace Commands {

class RTValComplexArg {
  /**
    RTValComplexArg defines a workflow where an command argument which
    type is RTVal needs to be get/set from several manners.

    RTValComplexArg arguments wraps the KL RTVal and allows to get/set
    either the value of the KL RTVal or the value of one of its member.
  */

  public:
    virtual ~RTValComplexArg() {}

    /// Gets the type of the RTVal associated
    /// with the complex arguement.
    virtual QString getType() = 0;

    /// Checks if the complex arg has these flags.
    /// Uses to identify the arg in the registry.
    virtual bool hasFlags(
      int flags
      ) = 0;

    /// Check
    virtual bool isTypeOf(
      const QString &json
      ) = 0;

    /// Sets the complex arg value.
    virtual QString setValue(
      const QString &key, 
      FabricCore::RTVal value, 
      FabricCore::RTVal &arg
      ) = 0;

    /// Sets the complex arg RTVal type.
    virtual QString getValueType(
      const QString &key, 
      const QString &type 
      ) = 0;

    /// Gets the complex arg value.
    virtual FabricCore::RTVal getValue(
      const QString &key, 
      FabricCore::RTVal arg 
      ) = 0;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMPLEX_ARG__
