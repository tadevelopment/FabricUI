//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <iostream>
#include "DFGPathValueResolver.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricServices/Persistence/RTValToJSONEncoder.hpp>

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

  bool res= knownBinding && 
    getDFGType(pathValue) != DFGUnknow;

  std::cout << "\nDFGPathValueResolver::knownPath " << path.toUtf8().constData() << " " << res << std::endl;

  return res;
}

QString DFGPathValueResolver::getType(
  RTVal pathValue)
{
  QString type;

  FABRIC_CATCH_BEGIN();

  if(getDFGType(pathValue) == DFGVar)
    type = m_binding.getExec().getVarType(
      getRelativePath(pathValue).toUtf8().constData());

  else
  {
    QString portPath;
    DFGExec subExec = getSubExecAndPortPath(
      pathValue, 
      portPath);

    type = subExec.getPortResolvedType(
      portPath.toUtf8().constData()
      );
  }

  std::cout << "\nDFGPathValueResolver::getType " << getRelativePath(pathValue).toUtf8().constData() << " type " << type.toUtf8().constData() << std::endl;

  FABRIC_CATCH_END("DFGPathValueResolver::getType");

  return type;
}

void DFGPathValueResolver::getValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
    
  RTVal value;

  DFGType dfgType = getDFGType(pathValue);

  std::cout 
    << "\nDFGPathValueResolver::getValue 1 " 
    << getRelativePath(pathValue).toUtf8().constData() 
    << " "
    << bool(dfgType == DFGPort)
    << std::endl;

  if(dfgType == DFGVar)
    value = m_binding.getExec().getVarValue(
      getRelativePath(pathValue).toUtf8().constData());

  else 
  {
    DFGPortPaths dfgPortPaths;
    DFGExec subExec = getSubExecAndPortPaths(
      pathValue,
      dfgPortPaths);

    if(dfgType == DFGPort)
      value = subExec.getPortResolvedDefaultValue( 
        dfgPortPaths.getRelativePortPath().toUtf8().constData(), 
        subExec.getPortResolvedType(dfgPortPaths.getRelativePortPath().toUtf8().constData())
        );

    else if(dfgType == DFGArg)
      value = m_binding.getArgValue(
        dfgPortPaths.getRelativePortPath().toUtf8().constData());
  }

  if(value.isValid())
    pathValue.setMember("value", value);
  else
    std::cout << "Invalid value at " << getRelativePath(pathValue).toUtf8().constData() << std::endl;

   std::cout 
    << "DFGPathValueResolver::getValue 2 " 
    << RTValUtil::getType(value).toUtf8().constData() 
    << std::endl;

  castPathToHRFormat( pathValue);

  FABRIC_CATCH_END("DFGPathValueResolver::getValue");
}

void DFGPathValueResolver::setValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
  std::cout << "\nDFGPathValueResolver::setValue " << getRelativePath(pathValue).toUtf8().constData() << std::endl;

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
    DFGPortPaths dfgPortPaths;
    DFGExec subExec = getSubExecAndPortPaths(
      pathValue,
      dfgPortPaths);

    if(dfgType == DFGPort)
      subExec.setPortDefaultValue( 
        dfgPortPaths.getRelativePortPath().toUtf8().constData(), 
        value, 
        false);

    else if( dfgType == DFGArg )
    {
      
      { // Code copy-pasted from FabricUI/DFG/DFGUICmd_SetArgValue.cpp :

        // Automatically set as "persistable" arg values that were explicitly set by the user
        // NOTE: metadata additions are not properly undone, however in this case it's not a big issue
        //       since we mostly want to avoid "too big values" like meshes to be persisted, which
        //       shouldn't be the case here.
        m_binding.getExec().setExecPortMetadata(
          dfgPortPaths.getRelativePortPath().toUtf8().constData(),
          DFG_METADATA_UIPERSISTVALUE,
          "true",
          false
        );
      }

      m_binding.setArgValue(
        dfgPortPaths.getRelativePortPath().toUtf8().constData(),
        value,
        false );
    }
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
      path = path.mid(index);
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

    std::cout 
      << "DFGPathValueResolver::getDFGType 1 " 
      << path.toUtf8().constData()
      << std::endl;

    if(m_binding.getExec().hasVar(
        path.toUtf8().constData()
      ))
      return DFGVar;

    std::cout << "DFGPathValueResolver::getDFGType 2 " << std::endl;

    DFGPortPaths dfgPortPaths;
    DFGExec subExec = getSubExecAndPortPaths(
      pathValue,
      dfgPortPaths);

    if(dfgPortPaths.isExecArg() && subExec.haveExecPort(dfgPortPaths.portName.toUtf8().constData()))
      return DFGArg; 

    else
    {
      if(dfgPortPaths.isExecBlockPort())
        subExec = subExec.getInstBlockExec(
          dfgPortPaths.nodeName.toUtf8().constData(), 
          dfgPortPaths.blockName.toUtf8().constData()
          );
      else
        subExec = subExec.getSubExec(dfgPortPaths.nodeName.toUtf8().constData());

      if(subExec.haveExecPort(dfgPortPaths.portName.toUtf8().constData()))
        return DFGPort;
    }
  }

  catch(Exception &e)
  {
    std::cout 
      << "DFGPathValueResolver::getDFGType 3 " 
      << e.getDesc_cstr()
      << std::endl;

    return DFGUnknow;
  }

  catch(FabricException &e)
  {
    std::cout 
      << "DFGPathValueResolver::getDFGType 4 " 
      << e.what()
      << std::endl;

    return DFGUnknow;
  }

  return DFGUnknow;
}
 
DFGExec DFGPathValueResolver::getSubExecAndPortPath(
  RTVal pathValue, 
  QString &portPath)  
{
  DFGPortPaths dfgPortPaths;
  DFGExec exec = getSubExecAndPortPaths(pathValue, dfgPortPaths);
  portPath = dfgPortPaths.nodeName + "." + dfgPortPaths.portName;
  return exec;
}

DFGExec DFGPathValueResolver::getSubExecAndPortPaths(
  RTVal pathValue, 
  DFGPortPaths &dfgPortPaths)  
{
  QString path = getRelativePath(pathValue);
  DFGExec exec;

  FABRIC_CATCH_BEGIN();

  int index = path.lastIndexOf(".");
  exec = m_binding.getExec();

  if(index != -1)
  {
    QString nodePath = path.mid(0, index);
    dfgPortPaths.portName = path.mid(index+1);

    index = nodePath.lastIndexOf(".");
    dfgPortPaths.subExecPath = (index != -1)
      ? nodePath.mid(0, index-1)
      : ".";

    std::cout 
      << "DFGPathValueResolver::getSubExecAndPortPath " 
      << " path "
      << path.toUtf8().constData()
      << " nodePath "
      << nodePath.toUtf8().constData()
      << " portName "
      << dfgPortPaths.portName.toUtf8().constData()
      << " isInstBlock "
      << m_binding.getExec().isInstBlock( nodePath.toUtf8().constData())
      << std::endl;

    if(m_binding.getExec().isInstBlock( nodePath.toUtf8().constData()))
    {
      index = dfgPortPaths.subExecPath.lastIndexOf(".");
        dfgPortPaths.subExecPath = (index == -1)
          ? "."
          : dfgPortPaths.subExecPath.mid(0, index-1);

      index = nodePath.lastIndexOf(".");
      dfgPortPaths.blockName = nodePath.mid(index+1);
      QString temp = nodePath.mid(0, index);
      
      index = temp.lastIndexOf(".");
      dfgPortPaths.nodeName = temp.mid(index+1);
    }

    else
    {
      index = nodePath.lastIndexOf(".");
      dfgPortPaths.nodeName = nodePath.mid(index+1);
    }
  }
  else
  {    
    dfgPortPaths.subExecPath = ".";
    dfgPortPaths.portName = path;
  }

  std::cout 
    << " portName "
    << dfgPortPaths.portName.toUtf8().constData()
    << " blockName "
    << dfgPortPaths.blockName.toUtf8().constData()
    << " nodeName "
    << dfgPortPaths.nodeName.toUtf8().constData()
    << " subExecPath "
    << dfgPortPaths.subExecPath.toUtf8().constData()
    << " block "
    << dfgPortPaths.isExecBlockPort()
    << std::endl;

  exec = m_binding.getExec().getSubExec(
    dfgPortPaths.subExecPath.toUtf8().constData()
    );

  FABRIC_CATCH_END("DFGPathValueResolver::getSubExecAndPortPath");

  return exec;
}

void DFGPathValueResolver::castPathToHRFormat(
  FabricCore::RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  QString path = !m_id.isEmpty() 
    ? m_id + "." + getRelativePath(pathValue)
    : "." + getRelativePath(pathValue);

  RTVal pathVal = RTVal::ConstructString(
    pathValue.getContext(),
    path.toUtf8().constData()
    );
  
  pathValue = RTValUtil::toRTVal(pathValue);
  pathValue.setMember("path", pathVal);

  FABRIC_CATCH_END("DFGPathValueResolver::castPathToHRFormat");
}
