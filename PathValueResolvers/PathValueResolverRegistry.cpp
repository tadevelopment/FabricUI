// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "PathValueResolverRegistry.h"
 
using namespace FabricUI;
using namespace FabricCore;
using namespace PathValueResolvers;

bool PathValueResolverRegistry::s_instanceFlag = false;
PathValueResolverRegistry* PathValueResolverRegistry::s_registry = 0;

PathValueResolverRegistry::PathValueResolverRegistry()
{
}

PathValueResolverRegistry::~PathValueResolverRegistry()
{
  s_instanceFlag = false;
}

PathValueResolverRegistry* PathValueResolverRegistry::GetRegistry()
{
  if(s_instanceFlag == false)
  {
  	s_registry = new PathValueResolverRegistry();
  	s_instanceFlag = true;
  }
  return s_registry;
}

void PathValueResolverRegistry::registerResolver(
	const QString name,
  PathValueResolver *resolver)
{
	if(!hasResolver(name))
		m_registeredSolvers[name] = resolver;
}

void PathValueResolverRegistry::unregisterResolver(
  const QString name)
{
	if(hasResolver(name))
		m_registeredSolvers.remove(name);
}

bool PathValueResolverRegistry::hasResolver(
  const QString name)
{
	return m_registeredSolvers.count(name);
}

PathValueResolver* PathValueResolverRegistry::getResolver(
  const QString name)
{
	return hasResolver(name) 
		? m_registeredSolvers[name]
		: 0;
}
 
bool PathValueResolverRegistry::knownPath(
	RTVal pathValue)
{
	foreach(PathValueResolver* resolver, m_registeredSolvers)
	{
		if(resolver->knownPath(pathValue))
			return true;
	}
	return false;
}

QString PathValueResolverRegistry::getType(
	RTVal pathValue)
{
	foreach(PathValueResolver* resolver, m_registeredSolvers)
	{
		if(resolver->knownPath(pathValue))
			return resolver->getType(pathValue);
	}
	return "";
}

void PathValueResolverRegistry::getValue(
	RTVal pathValue)
{
	foreach(PathValueResolver* resolver, m_registeredSolvers)
	{
		if(resolver->knownPath(pathValue))
			resolver->getValue(pathValue);
	}
}

void PathValueResolverRegistry::setValue(
	RTVal pathValue)
{
	foreach(PathValueResolver* resolver, m_registeredSolvers)
	{
		if(resolver->knownPath(pathValue))
			resolver->setValue(pathValue);
	}
}
