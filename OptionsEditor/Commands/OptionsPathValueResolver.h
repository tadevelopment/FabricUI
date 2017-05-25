//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPTIONS_PATH_VALUE_RESOLVER__
#define __UI_OPTIONS_PATH_VALUE_RESOLVER__

#include <FabricUI/Commands/PathValueResolver.h>

namespace FabricUI {
namespace OptionsEditor {

class OptionsPathValueResolver : public Commands::PathValueResolver
{
  /**
    OptionsPathValueResolver  
  */  
 
  public:
    OptionsPathValueResolver();

    virtual ~OptionsPathValueResolver();
    
    virtual void registrationCallback(
      void *userData
      );

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
