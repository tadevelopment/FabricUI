//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CreateToolCommand.h"
#include <FabricUI/Util/QtUtil.h>
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricUI;
using namespace Util;
using namespace Tools;
using namespace Commands;
using namespace Application;
using namespace FabricCore;

CreateToolCommand::CreateToolCommand() 
  : BaseRTValScriptableCommand()
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg("type", "String");

  declareRTValArg("path", "String");

  FABRIC_CATCH_END("CreateToolCommand::CreateToolCommand");
}

CreateToolCommand::~CreateToolCommand() 
{
}

bool CreateToolCommand::canUndo()
{
  return false;
}

bool CreateToolCommand::canLog()
{
  return true;
}

inline RTVal getAppToolRegistry()
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

  FABRIC_CATCH_END("CreateToolCommand::getAppToolRegistry");

  return toolRegistry;
}

bool CreateToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  RTVal typeAsStr = getRTValArgValue("type");
  RTVal path = RTValUtil::toKLRTVal(getRTValArgValue("path"));

  RTVal obj = RTVal::Construct(
    Application::FabricApplicationStates::GetAppStates()->getContext(), 
    typeAsStr.getStringCString(), 
    0, 0);

  RTVal objType = obj.callMethod("Type", "type", 0, 0);

  RTVal args[3] = { 
    objType, 
    path,
    RTVal::ConstructBoolean(Application::FabricApplicationStates::GetAppStates()->getContext(), true)
  };

  RTVal tool = getAppToolRegistry().callMethod(
    "Tool::BaseTool",
    "createPathValueTool",
    3,
    args);

  m_uid = tool.callMethod("UInt64", "uid", 0, 0);
 
  return true;

  FABRIC_CATCH_END("CreateToolCommand::doIt");

  return false;
}

QString CreateToolCommand::getHelp()
{
  FABRIC_CATCH_BEGIN();

  QMap<QString, QString> argsHelp;
  argsHelp["type"] = "Type of the tool (KL)";
  argsHelp["path"] = "Tool arguments";

  return CommandHelpers::createHelpFromRTValArgs(
    this,
    "Create a new tool",
    argsHelp);

  FABRIC_CATCH_END("CreateToolCommand::getHelp");

  return "";
}
