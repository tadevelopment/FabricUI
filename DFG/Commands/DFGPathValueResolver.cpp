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
  onSetEvalContextID(m_binding.getMetadata("maya_id"));
}

DFGPathValueResolver::DFGType DFGPathValueResolver::checkDFGType(
  RTVal pathValue) 
{
  try 
  {
    QString path = getPathValuePath(pathValue);

    if(m_binding.getExec().hasVar(
        path.toUtf8().constData()
      ))
      return DFGVar;
 
    int index = path.lastIndexOf(".");
    if(index != -1)
    {
      DFGExec exec = m_binding.getExec().getSubExec(
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
  return !m_evalContextID.isEmpty()
    ? BasePathValueResolver::knownPath(pathValue) && checkDFGType(pathValue) != DFGUnknow
    : checkDFGType(pathValue) != DFGUnknow;
}

QString DFGPathValueResolver::getType(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  QString path = getPathValuePath(pathValue);
  DFGExec exec = m_binding.getExec();

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
  QString path = getPathValuePath(pathValue);
  DFGExec exec = m_binding.getExec();
  
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
  QString path = getPathValuePath(pathValue);

  RTVal value = RTValUtil::toRTVal(
    pathValue.maybeGetMember("value"));

  if( !value.isValid() )
    return; // no value specified

  // TODO: properly support empty RTVal values (see comment in PathValue.kl)
  if( value.isString() ) {
    std::string str( value.getStringCString() );
    if( str == "!empty!" )
      return; // no value specified
  }

  DFGExec exec = m_binding.getExec();

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
