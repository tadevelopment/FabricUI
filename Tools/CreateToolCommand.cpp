//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "AppTool.h"
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

  declareRTValArg("args", "String");

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
  return false;
}

inline RTVal getAppToolRegistry()
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

  FABRIC_CATCH_END("CreateToolCommand::getAppToolRegistry");

  return toolRegistry;
}

bool CreateToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  RTVal type = getRTValArgValue("type");//.getStringCString();
  RTVal args = RTValUtil::toKLRTVal(getRTValArgValue("args"));

  RTVal appToolRegistry = getAppToolRegistry();

  RTVal temp = RTVal::Construct(appToolRegistry.getContext(), type.getStringCString(), 0, 0);
  RTVal tempType = temp.callMethod("Type", "type", 0, 0);

  AppTool *tool = new AppTool();
  tool->createKLTool(tempType, args);
  // if(appToolRegistry.callMethod("Boolean", "isRegistered", 1, &tempType))
  // { 
  //   RTVal temp[2] = { tempType, args };
  //   appToolRegistry.callMethod("", "createTool", 2, temp);
  // }

  return true;

  FABRIC_CATCH_END("CreateToolCommand::doIt");

  return false;
}

QString CreateToolCommand::getHelp()
{
  FABRIC_CATCH_BEGIN();

  QMap<QString, QString> argsHelp;
  argsHelp["type"] = "Type of the tool (KL)";
  argsHelp["args"] = "Tool arguments";

  return CommandHelpers::createHelpFromRTValArgs(
    this,
    "Create a new tool",
    argsHelp);

  FABRIC_CATCH_END("CreateToolCommand::getHelp");

  return "";
}
