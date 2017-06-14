//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_PATH_RESOLVER_REGISTRY__
#define __UI_PATH_RESOLVER_REGISTRY__

#include <QMap>
#include "BasePathValueResolver.h"
#include <FabricUI/Util/Factory.h>

namespace FabricUI {
namespace Commands {

class PathValueResolverRegistry : public Util::BaseFactoryRegistry
{  
  /**
    PathValueResolverRegistry  
  */  
  
  Q_OBJECT

  public:
    PathValueResolverRegistry();

    virtual ~PathValueResolverRegistry();

    /// Gets the resolver singleton.
    static PathValueResolverRegistry* GetRegistry();
    
    /// Registers a resolver.
    bool hasResolver(
      QString name
      );

    /// Registers a resolver.
    BasePathValueResolver* getOrCreateResolver(
      QString name
      );
      
    bool knownPath(
      FabricCore::RTVal pathValue
      );

    QString getType(
      FabricCore::RTVal pathValue
      );

    void getValue(
      FabricCore::RTVal pathValue
      );

    void setValue(
      FabricCore::RTVal pathValue
      );

  private:
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
    /// PathResolver singleton.
    static PathValueResolverRegistry *s_registry;
    /// PathResolver singleton.
    QMap<QString, BasePathValueResolver*> m_registeredResolvers;
};

template<typename T> 
class PathValueResolverFactory : public Util::TemplateFactory<T>
{
  /**
    PathValueResolverFactory is used to register PathValueResolvers in the PathValueResolverRegistry.
    - Register a command: PathValueResolverFactory<cmdType>::Register(name, userData);
  */
  public:
    PathValueResolverFactory(
      void *userData) 
      : Util::TemplateFactory<T>(userData) 
    {
    }

    /// Registers the command <T> under "name".
    static void Register(
      QString name,
      void *userData=0) 
    {
      Util::TemplateFactory<T>::Register(
        PathValueResolverRegistry::GetRegistry(),
        name,
        userData);

      // Creates the resolver when it's registered.
      PathValueResolverRegistry::GetRegistry()->getOrCreateResolver(
        name);
    }
};

} // namespace PathResolvers
} // namespace FabricUI

#endif // __UI_PATH_RESOLVER_REGISTRY__
