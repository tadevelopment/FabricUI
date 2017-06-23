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

inline DFGExec getSubExecAndRelativePortPath(
  DFGExec exec,
  RTVal pathValue, 
  QString &relPortPath)  
{
  QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
    "path").getStringCString();
  
  int index = path.lastIndexOf(".");
  if(index != -1)
  {
    QString temp = path.mid(0, index);
    int index2 = temp.lastIndexOf(".");
    if(index2 != -1)
    {
      relPortPath =  temp.mid(index2+1) + "." + path.mid(index+1);
      return exec.getSubExec(
        temp.midRef(0, index2).toUtf8().constData()
        );
    }
  }

  relPortPath = path;
  return exec;
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

    QString relPortPath;
    DFGExec subExec = getSubExecAndRelativePortPath(
      m_controller->getBinding().getExec(),
      pathValue,
      relPortPath);

    int index = relPortPath.lastIndexOf(".");
    if(index != -1)
    {
      subExec = subExec.getSubExec(relPortPath.midRef(0, index).toUtf8().constData());
      relPortPath = relPortPath.midRef(index+1).toUtf8().constData();
    }

    if(subExec.haveExecPort(relPortPath.toUtf8().constData()))
      return DFGPort;
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

  if(checkDFGType(pathValue) == DFGVar)
  {
    QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
      "path").getStringCString();

    return m_controller->getBinding().getExec().getVarType(
      path.toUtf8().constData());
  }

  else
  {
    QString relPortPath;
    DFGExec subExec = getSubExecAndRelativePortPath(
      m_controller->getBinding().getExec(),
      pathValue,
      relPortPath);

    return  subExec.getPortResolvedType(
      relPortPath.toUtf8().constData());
  }

  FABRIC_CATCH_END("DFGPathValueResolver::getType");
 
  return "";
}

void DFGPathValueResolver::getValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
  
  RTVal value;

  if(checkDFGType(pathValue) == DFGVar)
  {
    QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
      "path").getStringCString();

    value = m_controller->getBinding().getExec().getVarValue(
      path.toUtf8().constData());
  }

  else 
  {
    QString relPortPath;
    DFGExec subExec = getSubExecAndRelativePortPath(
      m_controller->getBinding().getExec(),
      pathValue,
      relPortPath);

    value = subExec.getPortResolvedDefaultValue( 
      relPortPath.toUtf8().constData(), 
      subExec.getPortResolvedType(relPortPath.toUtf8().constData())
      );
  }

  if(value.isValid())
    pathValue.setMember("value", value);

  FABRIC_CATCH_END("DFGPathValueResolver::getValue");
}

void DFGPathValueResolver::setValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  pathValue = RTValUtil::toRTVal(pathValue);
 
  RTVal value = RTValUtil::toRTVal(
    pathValue.maybeGetMember("value"));

  if( !value.isValid() )
    return; // no value specified

  if(checkDFGType(pathValue) == DFGVar)
  {
    QString path = pathValue.maybeGetMember(
      "path").getStringCString();

    m_controller->getBinding().getExec().setVarValue( 
      path.toUtf8().constData(), 
      value);
  }

  else
  {
    QString relPortPath;
    DFGExec subExec = getSubExecAndRelativePortPath(
      m_controller->getBinding().getExec(),
      pathValue,
      relPortPath);

    subExec.setPortDefaultValue( 
      relPortPath.toUtf8().constData(), 
      value, 
      false);
  }

  FABRIC_CATCH_END("DFGPathValueResolver::setValue");
}
