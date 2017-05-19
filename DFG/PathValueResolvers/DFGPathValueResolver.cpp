// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DFGPathValueResolver.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Util/FabricException.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace FabricCore;
using namespace PathValueResolvers;
 
DFGPathValueResolver::DFGPathValueResolver()
{
}

DFGPathValueResolver::~DFGPathValueResolver()
{
}
 
void DFGPathValueResolver::setBinding(
  DFGBinding binding)
{
  m_binding = binding;
}

bool DFGPathValueResolver::knownPath(
  RTVal pathValue)
{
  bool hasPort = false;

  try 
  {
    pathValue = RTValUtil::forceToRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();

    int index = path.lastIndexOf(".");
 
    DFGExec exec = m_binding.getExec().getSubExec(
      path.midRef(0, index).toUtf8().constData()
      );

    hasPort = exec.haveExecPort(
      path.midRef(index+1).toUtf8().constData()
      );
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "DFGPathValueResolver::knownPath",
      "",
      e.getDesc_cstr());
  }

  return hasPort;
}

QString DFGPathValueResolver::getType(
  RTVal pathValue)
{
  QString portType;

  try 
  {
    pathValue = RTValUtil::forceToRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();
    
    int index = path.lastIndexOf(".");
 
    DFGExec exec = m_binding.getExec().getSubExec(
      path.midRef(0, index).toUtf8().constData()
      );

    portType = exec.getPortTypeSpec(
      path.midRef(index+1).toUtf8().constData()
      );
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "DFGPathValueResolver::getType",
      "",
      e.getDesc_cstr());
  }

  return portType;
}

void DFGPathValueResolver::getValue(
  RTVal pathValue)
{
  try 
  {
    pathValue = RTValUtil::forceToRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();

    RTVal value = m_binding.getExec().getPortDefaultValue( 
      path.toUtf8().constData(), 
      getType(pathValue).toUtf8().constData()
      );

    pathValue.setMember(
      "value", 
      value);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "DFGPathValueResolver::getValue",
      "",
      e.getDesc_cstr()
      );
  }
}

void DFGPathValueResolver::setValue(
  RTVal pathValue)
{
  try
  {
    pathValue = RTValUtil::forceToRTVal(
      pathValue);

    QString path = pathValue.maybeGetMember(
      "path").getStringCString();

    RTVal value = pathValue.maybeGetMember(
      "value");

    value = RTValUtil::forceToRTVal(
      value);

    m_binding.getExec().setPortDefaultValue( 
      path.toUtf8().constData(), 
      value, 
      false);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "DFGPathValueResolver::setValue",
      "",
      e.getDesc_cstr());
  }
}
