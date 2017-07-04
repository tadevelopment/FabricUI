//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "AppTool.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Commands/KLCommandRegistry.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;
using namespace Tools;

AppTool::AppTool()
  //BaseViewItem *viewItem)
  //: m_viewItem(viewItem)
  : m_checkbox(0)
{
  // connect(
  //   viewItem,
  //   SIGNAL(toggleManipulation(bool)),
  //   this,
  //   SLOT(onToggleManipulation(bool))
  //   );
}

AppTool::~AppTool()
{
  try
  {
    if(m_klTool.isValid() && !m_klTool.isNullObject())
    {
      RTVal baseTool = RTVal::Create(
        m_klTool.getContext(),
        "Tool::BaseTool",
        1,
        &m_klTool
        );

      // To prevent possible leacks.
      baseTool.callMethod(
        "", 
        "invalidate", 
        0, 
        0
        );

      m_klTool = RTVal();
    }
  }

  catch(Exception &e)
  {
    printf(
      "AppTool::~AppTool: exception: %s\n", 
      e.getDesc_cstr()
      );
  }
}

RTVal AppTool::getAppToolRegistry()
{
  RTVal toolRegistry;

  try
  {    
    toolRegistry = RTVal::Create(
      Application::FabricApplicationStates::GetAppStates()->getContext(),
      "Tool::AppToolRegistry",
      0,
      0
      );

    toolRegistry = toolRegistry.callMethod(
      "Tool::AppToolRegistry",
      "getAppToolRegistry",
      0,
      0
      );

    toolRegistry.callMethod(
      "",
      "registerAppTools",
      0,
      0
      );

    toolRegistry.callMethod(
      "",
      "registerAppTargets",
      0,
      0
      );
  }

  catch(Exception &e)
  {
    printf(
      "AppTool::getAppToolRegistry: exception: %s\n", 
      e.getDesc_cstr()
      );
  }

  return toolRegistry;
}

QCheckBox* AppTool::createKLTool(
  FabricCore::RTVal drivenDataType)
{
  try
  {    
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

      QObject::connect(
        m_checkbox,
        SIGNAL(toggled(bool)),
        this,
        SLOT(setVisible(bool))
        );

      return m_checkbox;
    }
  }

  catch(Exception &e)
  {
    printf(
      "AppTool::createKLTool: exception: %s\n", 
      e.getDesc_cstr()
      );
  }

  return 0;
}

void AppTool::setVisible(
  bool visibility)
{
  try
  {
    RTVal visibilityVal = RTVal::ConstructBoolean(
      m_klTool.getContext(),
      visibility
      );
    
    RTVal baseTool = RTVal::Create(
      m_klTool.getContext(),
      "Tool::BaseTool",
      1,
      &m_klTool
      );

    baseTool.callMethod(
      "",
      "setVisible",
      1,
      &visibilityVal
      );

    //m_viewItem->emitRefreshViewport();
  }

  catch(Exception &e)
  {
    printf(
      "AppTool::setVisible: exception: %s\n", 
      e.getDesc_cstr());
  }
}
 
void AppTool::valueChanged(
  RTVal val) 
{
  try
  {
    if(m_klTool.isValid() && !m_klTool.isNullObject())
    {
      RTVal rtVal = Util::RTValUtil::toKLRTVal(
        val
        );

      m_klTool.callMethod(
        "",
        "valueChanged",
        1,
        &rtVal
        );
    }
  }

  catch(Exception &e)
  {
    printf(
      "AppTool::valueChanged: exception: %s\n", 
      e.getDesc_cstr()
      );
  }
}

void AppTool::onToggleManipulation(bool toggled) {
  if(m_checkbox)
    m_checkbox->setEnabled(toggled);
}
