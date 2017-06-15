//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

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
  m_controller = static_cast<DFGController*>(userData);
}

DFGPathValueResolver::DFGType DFGPathValueResolver::checkDFGType(
  RTVal pathValue) 
{
  try 
  {
    QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
      "path").getStringCString();

    if(m_controller->getBinding().getExec().hasVar(
        path.toUtf8().constData()
      ))
      return DFGVar;
 
    int index = path.lastIndexOf(".");
    if(index != -1)
    {
      DFGExec exec = m_controller->getBinding().getExec().getSubExec(
        path.midRef(0, index).toUtf8().constData()
        );

      if(exec.haveExecPort(
        path.midRef(index+1).toUtf8().constData()
        ))
      return DFGPort;
    }
  }

  catch(Exception &e)
  {
    return DFGUnknow;
  }

  return DFGUnknow;
}

bool DFGPathValueResolver::knownPath(
  RTVal pathValue)
{
  return checkDFGType(pathValue) != DFGUnknow;
}

QString DFGPathValueResolver::getType(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
    "path").getStringCString();
  
  DFGExec exec = m_controller->getBinding().getExec();

  return checkDFGType(pathValue) == DFGVar 
    ? exec.getVarType(path.toUtf8().constData())
    : exec.getPortResolvedType(path.toUtf8().constData());

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
 
  DFGExec exec = m_controller->getBinding().getExec();
  
  RTVal value = checkDFGType(pathValue) == DFGVar 
    ? exec.getVarValue(path.toUtf8().constData())
    : exec.getPortResolvedDefaultValue( 
        path.toUtf8().constData(), 
        exec.getPortResolvedType(path.toUtf8().constData())
        );
 
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

  if( !value.isValid() )
    return; // no value specified

  // TODO: properly support empty RTVal values (see comment in PathValue.kl)
  if( value.isString() ) {
    std::string str( value.getStringCString() );
    if( str == "(none)" )
      return; // no value specified
  }

  DFGExec exec = m_controller->getBinding().getExec();

  if(checkDFGType(pathValue) == DFGVar)
    exec.setVarValue( 
      path.toUtf8().constData(), 
      value);

  else
    exec.setPortDefaultValue( 
      path.toUtf8().constData(), 
      value, 
      false);

  FABRIC_CATCH_END("DFGPathValueResolver::setValue");
}
