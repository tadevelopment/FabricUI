//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CommandException.h"
#include "RTValComplexArgRegistry.h"
 
using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;

RTValComplexArgRegistry::RTValComplexArgRegistry() 
{
}
    
RTValComplexArgRegistry::~RTValComplexArgRegistry() 
{

}

void RTValComplexArgRegistry::registerArg(
  RTValComplexArg * arg) 
{
  m_registeredArgs.append(arg);
}

RTValComplexArg* RTValComplexArgRegistry::getRTValComplexArg(
  int flags)
{
  RTValComplexArg *registeredArg;
  foreach(registeredArg, m_registeredArgs)
  {
    if(registeredArg->hasFlags(flags))
      return registeredArg;
  }
  return 0;
}

bool RTValComplexArgRegistry::getRTValComplexArgType(
  int flags,
  QString &type)
{
  RTValComplexArg *registeredArg = getRTValComplexArg(flags);
  if(registeredArg)
  {
    type = registeredArg->getType();
    return true;
  }
  return false;
}

QString RTValComplexArgRegistry::setRTValComplexArgValue(
  int flags,
  QString key, 
  RTVal value, 
  RTVal &pathValue) 
{
  RTValComplexArg *registeredArg = getRTValComplexArg(flags);
  return registeredArg != 0
    ? registeredArg->setValue(key, value, pathValue)
    : "";
}

QString RTValComplexArgRegistry::getRTValComplexArgValueType(
  int flags,
  QString key, 
  QString type) 
{
  RTValComplexArg *registeredArg = getRTValComplexArg(flags);
  return registeredArg != 0
    ? registeredArg->getValueType(key, type)
    : "";
}

RTVal RTValComplexArgRegistry::getRTValComplexArgValue(
  int flags,
  QString key,
  RTVal pathValue)
{
  RTValComplexArg *registeredArg = getRTValComplexArg(flags);
  return registeredArg != 0
    ? registeredArg->getValue(key, pathValue)
    : "";
}

bool RTValComplexArgRegistry::isRTValComplexArgTypeOf(
  int flags,
  const QString &json)
{
  RTValComplexArg *registeredArg = getRTValComplexArg(flags);
  return registeredArg->isTypeOf(json);
}

