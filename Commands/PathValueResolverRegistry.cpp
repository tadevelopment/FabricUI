//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "PathValueResolverRegistry.h"
 
using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;

bool PathValueResolverRegistry::s_instanceFlag = false;
PathValueResolverRegistry* PathValueResolverRegistry::s_registry = 0;

PathValueResolverRegistry::PathValueResolverRegistry()
 : BaseFactoryRegistry()
{
  if(s_instanceFlag == false)
    s_instanceFlag = true;
}

PathValueResolverRegistry::~PathValueResolverRegistry()
{
  s_instanceFlag = false;
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver != 0)
    {
      delete resolver;
      resolver = 0;
    }
  }
  m_registeredResolvers.clear();
}

PathValueResolverRegistry* PathValueResolverRegistry::getRegistry()
{
  if(s_instanceFlag == false)
    s_registry = new PathValueResolverRegistry();
  return s_registry;
}

bool PathValueResolverRegistry::hasResolver(
  QString const&name)
{
  return m_registeredResolvers.count(name);
}

QString PathValueResolverRegistry::getResolverName(
  BasePathValueResolver* resolver)
{
  QMapIterator<QString, BasePathValueResolver*> it(m_registeredResolvers);
  while(it.hasNext()) 
  {
    it.next();
    if(resolver == it.value())
      return it.key();
  }
  return "";
}

BasePathValueResolver* PathValueResolverRegistry::getResolver(
  QString const&name)
{
  return hasResolver(name) ? m_registeredResolvers[name] : 0;
}
 
bool PathValueResolverRegistry::knownPath(
  RTVal pathValue)
{
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver->knownPath(pathValue))
      return true;
  }
  return false;
}

QString PathValueResolverRegistry::getType(
  RTVal pathValue)
{
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver->knownPath(pathValue))
      return resolver->getType(pathValue);
  }
  return "";
}

void PathValueResolverRegistry::getValue(
  RTVal pathValue)
{
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver->knownPath(pathValue))
      resolver->getValue(pathValue);
  }
}

void PathValueResolverRegistry::setValue(
  RTVal pathValue)
{
  foreach(BasePathValueResolver* resolver, m_registeredResolvers)
  {
    if(resolver->knownPath(pathValue))
      resolver->setValue(pathValue);
  }
}

void PathValueResolverRegistry::unregisterFactory(
  QString const&name)
{
  // unregister the factory
  BaseFactoryRegistry::unregisterFactory(name);

  // unregister the resolver
  if(hasResolver(name))
  {
    BasePathValueResolver* resolver = m_registeredResolvers[name];
    if(resolver != 0)
    {
      delete resolver;
      resolver = 0;
    }
    m_registeredResolvers.remove(name);
  }
}

void PathValueResolverRegistry::registerResolver(
  BasePathValueResolver *resolver,
  QString const&name)
{
  m_registeredResolvers[name] = resolver;
}