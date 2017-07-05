//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "AppTool.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Commands/KLCommandRegistry.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;
using namespace Tools;

AppTool::AppTool()
  : m_checkbox(0)
{
}

AppTool::~AppTool()
{
  FABRIC_CATCH_BEGIN();

  if(m_klTool.isValid() && !m_klTool.isNullObject())
  {
    RTVal baseTool = RTVal::Create(
      m_klTool.getContext(),
      "Tool::BaseTool",
      1,
      &m_klTool);

    // To prevent possible leacks.
    baseTool.callMethod(
      "", 
      "invalidate", 
      0, 
      0);

    m_klTool = RTVal();
  }

  FABRIC_CATCH_END("AppTool::~AppTool");
}

RTVal AppTool::getAppToolRegistry()
{
  RTVal toolRegistry;

  FABRIC_CATCH_BEGIN();

  toolRegistry = RTVal::Create(
    Application::FabricApplicationStates::GetAppStates()->getContext(),
    "Tool::AppToolsManager",
    0,
    0);

  toolRegistry = toolRegistry.callMethod(
    "Tool::ToolsManager",
    "getToolsManager",
    0,
    0);

  FABRIC_CATCH_END("AppTool::getAppToolRegistry");

  return toolRegistry;
}

QCheckBox* AppTool::createKLTool(
  RTVal drivenDataType,
  RTVal cmdArgs)
{
  FABRIC_CATCH_BEGIN();
  
  if(getAppToolRegistry().callMethod("Boolean", "isRegistered", 1, &drivenDataType) && cmdArgs.isValid())
  {
    RTVal toolArgs[2] = { drivenDataType, cmdArgs };

    m_klTool = getAppToolRegistry().callMethod(
      "Tool::AppTool",
      "createTool",
      2,
      toolArgs);

    // QString toolName = m_klTool.callMethod(
    //   "String",
    //   "type",
    //   0,
    //   0).getStringCString();

    //setVisible(false);
    
    // m_checkbox = new QCheckBox(
    //   toolName
    //   );
 
    // return m_checkbox;
  }

  FABRIC_CATCH_END("AppTool::createKLTool");

  return 0;
}

void AppTool::setVisible(
  bool visibility)
{
  FABRIC_CATCH_BEGIN();

  RTVal visibilityVal = RTVal::ConstructBoolean(
    m_klTool.getContext(),
    visibility);
  
  RTVal baseTool = RTVal::Create(
    m_klTool.getContext(),
    "Tool::BaseTool",
    1,
    &m_klTool);

  baseTool.callMethod(
    "",
    "setVisible",
    1,
    &visibilityVal);

  //m_viewItem->emitRefreshViewport();
  FABRIC_CATCH_END("AppTool::getAppToolRegistry");
}
 
void AppTool::valueChanged(
  RTVal val) 
{
  FABRIC_CATCH_BEGIN();

  if(m_klTool.isValid() && !m_klTool.isNullObject())
  {
    RTVal rtVal = Util::RTValUtil::toKLRTVal(
      val);

    m_klTool.callMethod(
      "",
      "valueChanged",
      1,
      &rtVal);
  }

  FABRIC_CATCH_END("AppTool::getAppToolRegistry");
}

void AppTool::onToggleManipulation(bool toggled) {
  if(m_checkbox)
    m_checkbox->setEnabled(toggled);
}
