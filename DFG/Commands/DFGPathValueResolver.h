//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_PATH_VALUE_RESOLVER__
#define __UI_DFG_PATH_VALUE_RESOLVER__

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/Commands/BasePathValueResolver.h>

namespace FabricUI {
namespace DFG {

class DFGPathValueResolver : public Commands::BasePathValueResolver
{
  /**
    DFGPathValueResolver  
  */  
  
  Q_OBJECT
  
  public:
    DFGPathValueResolver();

    virtual ~DFGPathValueResolver();
   
    /// Implementation of BasePathValueResolver.
    virtual void registrationCallback(
      void *userData
      );

    /// Implementation of BasePathValueResolver.
    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual void setValue(
      FabricCore::RTVal pathValue
      );

  private:
    ///
    DFGController *m_controller;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_PATH_VALUE_RESOLVER__
