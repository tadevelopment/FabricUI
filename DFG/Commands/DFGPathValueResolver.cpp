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

  return knownBinding && getDFGType(pathValue) != DFGUnknow;
}

QString DFGPathValueResolver::getType(
  RTVal pathValue)
{
  QString type;

  FABRIC_CATCH_BEGIN();

  if(getDFGType(pathValue) == DFGVar)
    type = m_binding.getExec().getVarType(
      getPathWithoutBindingOrSolverID(pathValue).toUtf8().constData());

  else
  {
    DFGPortPaths dfgPortPaths;
    DFGExec subExec = getDFGPortPaths(
      pathValue,
      dfgPortPaths);

    type = subExec.getPortResolvedType(
      dfgPortPaths.getRelativePortPath().toUtf8().constData()
      );
  }

  FABRIC_CATCH_END("DFGPathValueResolver::getType");

  return type;
}

void DFGPathValueResolver::getValue(
  RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();
    
  RTVal value;

  DFGType dfgType = getDFGType(pathValue);

  if(dfgType == DFGVar)
    value = m_binding.getExec().getVarValue(
      getPathWithoutBindingOrSolverID(pathValue).toUtf8().constData()
      );

  else 
  {
    DFGPortPaths dfgPortPaths;
    DFGExec subExec = getDFGPortPaths(
      pathValue,
      dfgPortPaths
      );

    if(dfgType == DFGPort)
      value = subExec.getPortResolvedDefaultValue( 
        dfgPortPaths.getRelativePortPath().toUtf8().constData(), 
        subExec.getPortResolvedType(dfgPortPaths.getRelativePortPath().toUtf8().constData())
        );

    else if(dfgType == DFGArg)
      value = m_binding.getArgValue(
        dfgPortPaths.getRelativePortPath().toUtf8().constData()
        );
  }

  QString type = RTValUtil::getType(value);
  
  if(value.isValid() && type != "None")
    pathValue.setMember("value", value);
  
  else
  {
    QString path = RTValUtil::toRTVal(pathValue).maybeGetMember(
      "path").getStringCString();

    FabricException::Throw(
      "DFGPathValueResolver::getValue",
      "Invalid PathValue : " + path + ", type " + type.toUtf8()
      );
  }

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
      getPathWithoutBindingOrSolverID(pathValue).toUtf8().constData(), 
      value);

  else
  {
    DFGPortPaths dfgPortPaths;
    DFGExec subExec = getDFGPortPaths(
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

QString DFGPathValueResolver::getPathWithoutBindingOrSolverID(
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
 
  FABRIC_CATCH_END("DFGPathValueResolver::getPathWithoutBindingOrSolverID");

  return path;
}

DFGPathValueResolver::DFGType DFGPathValueResolver::getDFGType(
  RTVal pathValue) 
{
  try 
  {
    QString path = getPathWithoutBindingOrSolverID(pathValue);;

    if(m_binding.getExec().hasVar(
        path.toUtf8().constData()
      ))
      return DFGVar;

    DFGPortPaths dfgPortPaths;
    DFGExec subExec = getDFGPortPaths(
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
    FabricException::Throw(
      "DFGPathValueResolver::getDFGType ",
      "Invalid PathValue",
      e.getDesc_cstr(),
      FabricException::LOG
      );

    return DFGUnknow;
  }

  catch(FabricException &e)
  {
    FabricException::Throw(
      "DFGPathValueResolver::getDFGType ",
      "Invalid PathValue",
      e.what(),
      FabricException::LOG
      );

    return DFGUnknow;
  }

  return DFGUnknow;
}
 
DFGExec DFGPathValueResolver::getDFGPortPaths(
  RTVal pathValue, 
  DFGPortPaths &dfgPortPaths)  
{
  DFGExec exec;
  QString path = getPathWithoutBindingOrSolverID(pathValue);

  FABRIC_CATCH_BEGIN();

  int index = path.lastIndexOf(".");

  QString subExecPath = ".";

  // Port (if Inst or Block)
  if(index != -1)
  {
    QString nodePath = path.mid(0, index);
    dfgPortPaths.portName = path.mid(index+1);

    index = nodePath.lastIndexOf(".");
    if(index != -1)
      subExecPath = nodePath.mid(0, index);

    // test block
    QString blockName, nodeName, subExecPath_;

    if(nodePath.count(".") >= 2)
    {
      index = subExecPath.lastIndexOf(".");
      if(index != -1)
        subExecPath_ = subExecPath.mid(0, index);

      index = nodePath.lastIndexOf(".");
      blockName = nodePath.mid(index+1);
      QString temp = nodePath.mid(0, index);
      
      index = temp.lastIndexOf(".");
      nodeName = temp.mid(index+1);
    }

    else
    { 
      index = nodePath.lastIndexOf(".");
      blockName = nodePath.mid(index+1);
      nodeName = nodePath.mid(0, index);
    }
    
    // Block inst
    if(m_binding.getExec().getSubExec(
      subExecPath_.toUtf8().constData()).isInstBlock(
        nodeName.toUtf8().constData(), 
        blockName.toUtf8().constData()))
    {
      subExecPath = subExecPath_;
      dfgPortPaths.blockName = blockName;
      dfgPortPaths.nodeName = nodeName;
    }

    // Inst
    else
    {
      index = nodePath.lastIndexOf(".");
      dfgPortPaths.nodeName = nodePath.mid(index+1);
    }

    exec = m_binding.getExec().getSubExec(
      subExecPath.toUtf8().constData()
      );
  }

  // Arg
  else
  {
    exec = m_binding.getExec();
    dfgPortPaths.portName = path;
  }

  // std::cout 
  //   << "DFGPathValueResolver::getDFGPortPaths " 
  //   << " portName "
  //   << dfgPortPaths.portName.toUtf8().constData()
  //   << " blockName "
  //   << dfgPortPaths.blockName.toUtf8().constData()
  //   << " nodeName "
  //   << dfgPortPaths.nodeName.toUtf8().constData()
  //   << " subExecPath "
  //   << subExecPath.toUtf8().constData()
  //   << " block "
  //   << dfgPortPaths.isExecBlockPort()
  //   << std::endl;

  FABRIC_CATCH_END("DFGPathValueResolver::getDFGPortPaths");

  return exec;
}

void DFGPathValueResolver::castPathToHRFormat(
  FabricCore::RTVal pathValue)
{
  FABRIC_CATCH_BEGIN();

  QString path = getPathWithoutBindingOrSolverID(pathValue);
  path = !m_id.isEmpty() ? m_id + "." + path : path;

  RTVal pathVal = RTVal::ConstructString(
    pathValue.getContext(),
    path.toUtf8().constData()
    );
  
  pathValue = RTValUtil::toRTVal(pathValue);
  pathValue.setMember("path", pathVal);

  FABRIC_CATCH_END("DFGPathValueResolver::castPathToHRFormat");
}
