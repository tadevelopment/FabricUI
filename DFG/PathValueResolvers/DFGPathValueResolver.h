//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_PATH_RESOLVER__
#define __UI_DFG_PATH_RESOLVER__

#include <QMap>
#include <FabricUI/PathValueResolvers/PathValueResolver.h>

namespace FabricUI {
namespace DFG {

class DFGPathValueResolver : public PathValueResolvers::PathValueResolver
{
  /**
    DFGPathValueResolver  
  */  
 
  public:
    DFGPathValueResolver();

    virtual ~DFGPathValueResolver();
   
    void setBinding(
      FabricCore::DFGBinding binding
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

  private:
    ///
    FabricCore::DFGBinding m_binding;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_PATH_RESOLVER__
