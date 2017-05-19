//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_PATH_RESOLVER_REGISTRY__
#define __UI_PATH_RESOLVER_REGISTRY__

#include <QMap>
#include "PathValueResolver.h"

namespace FabricUI {
namespace PathValueResolvers {

class PathValueResolverRegistry : public PathValueResolver 
{
  /**
    PathValueResolverRegistry  
  */  
 
  public:
    PathValueResolverRegistry();

    virtual ~PathValueResolverRegistry();

    /// Gets the resolver singleton.
    static PathValueResolverRegistry* GetRegistry();
    
    /// Registers a resolver.
    void registerResolver(
      const QString name,
      PathValueResolver *resolver
      );

    /// Unregisters a resolver.
    void unregisterResolver(
      const QString name
      );

    /// Registers a resolver.
    bool hasResolver(
      const QString name
      );

    /// Registers a resolver.
    PathValueResolver* getResolver(
      const QString name
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
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
    /// PathResolver singleton.
    static PathValueResolverRegistry *s_registry;
    /// List of registered solvers.
    QMap<QString, PathValueResolver*> m_registeredSolvers;
};

} // namespace PathResolvers
} // namespace FabricUI

#endif // __UI_PATH_RESOLVER_REGISTRY__
