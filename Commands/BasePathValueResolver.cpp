// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "BasePathValueResolver.h"
 
using namespace FabricUI;
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
