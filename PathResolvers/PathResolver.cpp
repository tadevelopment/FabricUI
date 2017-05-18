// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "PathResolver.h"
 
using namespace FabricUI;
using namespace FabricCore;
using namespace PathResolvers;

bool PathResolver::s_instanceFlag = false;
PathResolver* PathResolver::s_resolver = 0;

PathResolver::PathResolver()
{
  s_instanceFlag = true;
  s_resolver = this;
}

PathResolver::~PathResolver()
{
  s_instanceFlag = false;
}

PathResolver* PathResolver::GetPathResolver()
{
  return s_instanceFlag
    ? s_resolver
    : 0;
}
 
bool PathResolver::knownPath(
  FabricCore::RTVal pathValue)
{
  return false;
}

QString PathResolver::getType(
  FabricCore::RTVal pathValue)
{
  return "";
}

void PathResolver::getValue(
  FabricCore::RTVal pathValue)
{
}

void PathResolver::setValue(
  FabricCore::RTVal pathValue)
{
}
