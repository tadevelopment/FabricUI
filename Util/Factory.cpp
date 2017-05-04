//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "Factory.h"

using namespace FabricUI;
using namespace Util;

BaseFactoryRegistry::BaseFactoryRegistry() 
: QObject()
{
}

BaseFactoryRegistry::~BaseFactoryRegistry() 
{
  QMapIterator<QString, Factory*> it(m_factories);
  while(it.hasNext()) 
  {
    it.next();
    deleteFactory(it);
  }
}

Factory* BaseFactoryRegistry::getFactory(
  const QString &name) 
{
  return hasFactory(name) 
    ? m_factories[name] 
    : 0;
}

bool BaseFactoryRegistry::hasFactory(
  const QString &name) 
{
  return m_factories.count(name) > 0;
}

void BaseFactoryRegistry::registerFactory(
  const QString &name, 
  Factory *factory) 
{
  if(!hasFactory(name))
  {
    m_factories[name] = factory;
    emit factoryRegistered(
      name,
      factory);
  }
}

void BaseFactoryRegistry::unregisterFactory(
  const QString &name) 
{
  QMapIterator<QString, Factory*> it(m_factories);
  while(it.hasNext()) 
  {
    it.next();
    if(it.key() == name)
    {
    	deleteFactory(it);
      emit factoryUnregistered(name);
      break;
    }
  }
}

void BaseFactoryRegistry::deleteFactory(
	QMapIterator<QString, Factory*> &it) 
{
  Factory* factory = (Factory*)it.value();
  if(factory != 0)
  {
    delete factory;
    factory = 0;
  }
  m_factories.remove(it.key());
}
