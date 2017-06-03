// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DFGPathValueResolver.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
 
DFGPathValueResolver::DFGPathValueResolver()
 : BasePathValueResolver()
{
}

DFGPathValueResolver::~DFGPathValueResolver()
{
}
 
void DFGPathValueResolver::registrationCallback(
  void *userData)
{
  m_controller = (DFGController*)(userData);
}

bool DFGPathValueResolver::knownPath(
  RTVal pathValue)
{
  bool hasPort = false;

  try 
  {
    QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
      "path").getStringCString();

    int index = path.lastIndexOf(".");
    if(index != -1)
    {
      DFGExec exec = m_controller->getBinding().getExec().getSubExec(
        path.midRef(0, index).toUtf8().constData()
        );

      hasPort = exec.haveExecPort(
        path.midRef(index+1).toUtf8().constData()
        );
    }
  }

  catch(Exception &e)
  {
    hasPort = false;
  }

  return hasPort;
}

QString DFGPathValueResolver::getType(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
    "path").getStringCString();
  
  int index = path.lastIndexOf(".");

  DFGExec exec = m_controller->getBinding().getExec().getSubExec(
    path.midRef(0, index).toUtf8().constData()
    );

  return exec.getPortTypeSpec(
    path.midRef(index+1).toUtf8().constData()
    );

  FABRIC_CATCH_END("DFGPathValueResolver::getType");
 
  return "";
}

void DFGPathValueResolver::getValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  pathValue = RTValUtil::toRTVal(pathValue);

  QString path = pathValue.maybeGetMember(
    "path").getStringCString();

  RTVal value = m_controller->getBinding().getExec().getPortDefaultValue( 
    path.toUtf8().constData(), 
    getType(pathValue).toUtf8().constData());

  if(value.isValid())
    pathValue.setMember("value", value);

  FABRIC_CATCH_END("DFGPathValueResolver::getValue");
}

void DFGPathValueResolver::setValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  pathValue = RTValUtil::toRTVal(pathValue);

  QString path = pathValue.maybeGetMember(
    "path").getStringCString();

  RTVal value = RTValUtil::toRTVal(
    pathValue.maybeGetMember("value"));

  m_controller->getBinding().getExec().setPortDefaultValue( 
    path.toUtf8().constData(), 
    value, 
    false);

  FABRIC_CATCH_END("DFGPathValueResolver::setValue");
}
