//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "DFGPathValueResolver.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace DFG;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace Application;
 
DFGPathValueResolver::DFGPathValueResolver()
  : BasePathValueResolver()
  , m_id("")
{
}
 
DFGPathValueResolver::~DFGPathValueResolver()
{
}
 
void DFGPathValueResolver::registrationCallback(
  QString const&name,
  void *userData)
{
  if(userData != 0)
  {
    DFGController* controller = static_cast<DFGController*>(userData);
    onBindingChanged(controller->getBinding());
  
    QObject::connect(
      controller,
      SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
      this,
      SLOT(onBindingChanged(FabricCore::DFGBinding const &))
      );
  }
}

void DFGPathValueResolver::onBindingChanged(
  DFGBinding const &binding)
{
  m_binding = binding;
  m_id = binding.getMetadata("resolver_id");
}

bool DFGPathValueResolver::knownPath(
  RTVal pathValue)
{
  QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
    "path").getStringCString();
  
  bool knownBinding = false;

  int index = path.indexOf(".");
  if(index != -1)
  {
    QString id = path.mid(0, index);
    if(id == QString::number(m_binding.getBindingID()) || id == m_id)
      knownBinding = true;
  }
  knownBinding = !knownBinding ? m_id.isEmpty() : true;

  return knownBinding && 
    getDFGType(pathValue) != DFGUnknow;
}

QString DFGPathValueResolver::getType(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  if(getDFGType(pathValue) == DFGVar)
    return m_binding.getExec().getVarType(
      getRelativePath(pathValue).toUtf8().constData());

  else
  {
    QString portPath;
    DFGExec subExec = getSubExecAndPortPath(
      pathValue, 
      portPath);

    return subExec.getPortResolvedType(
      portPath.toUtf8().constData()
      );
  }

  FABRIC_CATCH_END("DFGPathValueResolver::getType");

  return "";
}

void DFGPathValueResolver::getValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
    
  RTVal value;
  QString path = getRelativePath(pathValue);

  DFGType dfgType = getDFGType(pathValue);

  if(dfgType == DFGVar)
    value = m_binding.getExec().getVarValue(
      getRelativePath(pathValue).toUtf8().constData());

  else 
  {
    QString portPath;
    DFGExec subExec = getSubExecAndPortPath(
      pathValue, 
      portPath);

    if(dfgType == DFGPort)
      value = subExec.getPortResolvedDefaultValue( 
        portPath.toUtf8().constData(), 
        subExec.getPortResolvedType(portPath.toUtf8().constData())
        );

    else if(dfgType == DFGArg)
      value = m_binding.getArgValue(
        portPath.toUtf8().constData());
  }

  if(value.isValid())
    pathValue.setMember("value", value);

  castPathToHRFormat( pathValue);

  FABRIC_CATCH_END("DFGPathValueResolver::getValue");
}

void DFGPathValueResolver::setValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
  
  RTVal value = RTValUtil::toRTVal(
    RTValUtil::toRTVal(pathValue).maybeGetMember("value"));

 
  if( !value.isValid() )
    return; // no value specified

  DFGType dfgType = getDFGType(pathValue);

  if(dfgType == DFGVar)
    m_binding.getExec().setVarValue( 
      getRelativePath(pathValue).toUtf8().constData(), 
      value);

  else
  {
    QString portPath;
    DFGExec subExec = getSubExecAndPortPath(
      pathValue,
      portPath);

    if(dfgType == DFGPort)
      subExec.setPortDefaultValue( 
        portPath.toUtf8().constData(), 
        value, 
        false);

    else if(dfgType == DFGArg)
      m_binding.setArgValue( 
        portPath.toUtf8().constData(), 
        value, 
        false);
  }

  FABRIC_CATCH_END("DFGPathValueResolver::setValue");
}

QString DFGPathValueResolver::getRelativePath(
  RTVal pathValue) 
{
  QString path;

  FABRIC_CATCH_BEGIN();

  path = RTValUtil::toRTVal(pathValue).maybeGetMember(
    "path").getStringCString();
  
  int index = path.indexOf(".");
  if(index != -1)
  {
    QString id = path.mid(0, index);
    if(id == QString::number(m_binding.getBindingID()) || id == m_id)
      path = path.mid(index+1);
  }
 
  FABRIC_CATCH_END("DFGPathValueResolver::getRelativePath");

  return path;
}

DFGPathValueResolver::DFGType DFGPathValueResolver::getDFGType(
  RTVal pathValue) 
{
  try 
  {
    QString path = getRelativePath(pathValue);

    if(m_binding.getExec().hasVar(
        path.toUtf8().constData()
      ))
      return DFGVar;

    QString portPath;
    DFGExec subExec = getSubExecAndPortPath(
      pathValue,
      portPath);

    int index = portPath.lastIndexOf(".");
    if(index != -1)
    {
      subExec = subExec.getSubExec(portPath.midRef(0, index).toUtf8().constData());
      portPath = portPath.midRef(index+1).toUtf8().constData();
    }

    if(subExec.haveExecPort(portPath.toUtf8().constData()))
      return index != -1 ? DFGPort : DFGArg;
  }

  catch(Exception &e)
  {
    return DFGUnknow;
  }

  return DFGUnknow;
}
 
DFGExec DFGPathValueResolver::getSubExecAndPortPath(
  RTVal pathValue, 
  QString &portPath)  
{
  QString path = getRelativePath(pathValue);

  FABRIC_CATCH_BEGIN();

  int index = path.lastIndexOf(".");
  if(index != -1)
  {
    QString temp = path.mid(0, index);
    int index2 = temp.lastIndexOf(".");
    if(index2 != -1)
    {
      portPath = temp.mid(index2+1) + "." + path.mid(index+1);
      return m_binding.getExec().getSubExec(
        temp.midRef(0, index2).toUtf8().constData()
        );
    }
  }

  FABRIC_CATCH_END("DFGPathValueResolver::getSubExecAndPortPath");
  
  portPath = path;
  return m_binding.getExec();
}

void DFGPathValueResolver::castPathToHRFormat(
  FabricCore::RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  QString path = !m_id.isEmpty() 
    ? m_id + "." + getRelativePath(pathValue)
    : getRelativePath(pathValue);

  RTVal pathVal = RTVal::ConstructString(
    pathValue.getContext(),
    path.toUtf8().constData()
    );
  
  pathValue = RTValUtil::toRTVal(pathValue);
  pathValue.setMember("path", pathVal);

  FABRIC_CATCH_END("DFGPathValueResolver::castPathToHRFormat");
}
