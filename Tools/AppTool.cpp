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
    "Tool::AppToolRegistry",
    0,
    0);

  toolRegistry = toolRegistry.callMethod(
    "Tool::AppToolRegistry",
    "getAppToolRegistry",
    0,
    0);

  toolRegistry.callMethod(
    "",
    "registerAppTools",
    0,
    0);

  toolRegistry.callMethod(
    "",
    "registerAppTargets",
    0,
    0);

  FABRIC_CATCH_END("AppTool::getAppToolRegistry");

  return toolRegistry;
}

QCheckBox* AppTool::createKLTool(
  FabricCore::RTVal drivenDataType)
{
  FABRIC_CATCH_BEGIN();
 
  QString cmdName;// = modelItem->getCommandName();
  RTVal cmdArgs;// = modelItem->getCommandArgs();

  if(cmdArgs.isValid())
  {
    RTVal toolArgs[2] =
    {
      drivenDataType,  
      cmdArgs
    };

    m_klTool = getAppToolRegistry().callMethod(
      "Tool::AppTool",
      "createTool",
      2,
      toolArgs
      );

    QString toolName = m_klTool.callMethod(
      "String",
      "type",
      0,
      0
      ).getStringCString();

    setVisible(false);
    
    m_checkbox = new QCheckBox(
      toolName
      );

    // QObject::connect(
    //   m_checkbox,
    //   SIGNAL(toggled(bool)),
    //   this,
    //   SLOT(setVisible(bool))
    //   );

    return m_checkbox;
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
