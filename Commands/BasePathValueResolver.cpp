//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BasePathValueResolver.h"
#include "PathValueResolverRegistry.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
 
BasePathValueResolver::BasePathValueResolver()
 : QObject()
{
}

BasePathValueResolver::~BasePathValueResolver()
{
}

void BasePathValueResolver::registrationCallback(
  QString const&name,
  void *userData)
{
}

bool BasePathValueResolver::knownPath(
  RTVal pathValue)
{
  return false;
}

QString BasePathValueResolver::getType(
  RTVal pathValue)
{
  return "";
}

void BasePathValueResolver::getValue(
  RTVal pathValue)
{
}

void BasePathValueResolver::setValue(
  RTVal pathValue)
{
}
