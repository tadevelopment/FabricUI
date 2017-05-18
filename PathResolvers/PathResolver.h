//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_PATH_RESOLVER__
#define __UI_PATH_RESOLVER__

#include <QString>
#include <FabricCore.h>
 
namespace FabricUI {
namespace PathResolvers {

class PathResolver
{
  /**
    PathResolver  
  */  
 
  public:
    PathResolver();

    virtual ~PathResolver();

    /// Gets the resolver singleton.
    static PathResolver* GetPathResolver();
  
    /// Implementation of PathResolver.
    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );

    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    ///
    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    virtual void setValue(
      FabricCore::RTVal pathValue
      );

  private:
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
    /// PathResolver singleton.
    static PathResolver *s_resolver;
};

} // namespace PathResolvers
} // namespace FabricUI

#endif // __UI_PATH_RESOLVER__
