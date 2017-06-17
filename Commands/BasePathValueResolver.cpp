//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BasePathValueResolver.h"
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
  void *userData)
{
}

bool BasePathValueResolver::knownPath(
  RTVal pathValue)
{
	bool res = false;

  FABRIC_CATCH_BEGIN();

  QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
    "path").getStringCString();

  int index = path.indexOf(".");
  res = index == -1
    ? false
    : path.mid(0, index) == m_evalContextID;

  FABRIC_CATCH_END("BasePathValueResolver::knownPath");

  return res;
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

void BasePathValueResolver::onSetEvalContextID(
 	QString evalContextID)
{
 	m_evalContextID = evalContextID;
}

QString BasePathValueResolver::getPathValuePath(
  RTVal pathValue) 
{
	QString path;

  FABRIC_CATCH_BEGIN();

  path = RTValUtil::toRTVal(pathValue).maybeGetMember(
    "path").getStringCString();

  if(!m_evalContextID.isEmpty())
  {
    int index = path.indexOf(".");
    path = path.mid(index+1);
  }
  
  FABRIC_CATCH_END("BasePathValueResolver::getPathValuePath");

	return path;
}