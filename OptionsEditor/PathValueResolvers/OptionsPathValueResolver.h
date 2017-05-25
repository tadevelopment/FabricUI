//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_PATH_VALUE_RESOLVER__
#define __UI_OPTIONS_PATH_VALUE_RESOLVER__

#include <FabricCore.h>
#include <FabricUI/PathValueResolvers/PathValueResolver.h>

namespace FabricUI {
namespace OptionsEditor {

class OptionsPathValueResolver : public PathValueResolvers::PathValueResolver
{
  /**
    OptionsPathValueResolver  
  */  
 
  public:
    OptionsPathValueResolver();

    virtual ~OptionsPathValueResolver();
   
    /// Implementation of PathValueResolver.
    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );

    /// Implementation of PathValueResolver.
    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    /// Implementation of PathValueResolver.
    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    /// Implementation of PathValueResolver.
    virtual void setValue(
      FabricCore::RTVal pathValue
      );
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_OPTIONS_PATH_VALUE_RESOLVER__
