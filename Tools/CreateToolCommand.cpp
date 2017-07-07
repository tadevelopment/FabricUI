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
using namespace FabricCore;
using namespace Application;

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

void CreateToolCommand::registrationCallback(
  QString const&name, 
  void *userData)
{
  if(userData != 0)
    m_manager = static_cast<ToolsManager*>(userData);
}

bool CreateToolCommand::canUndo()
{
  return false;
}

bool CreateToolCommand::canLog()
{
  return true;
}

bool CreateToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  RTVal typeAsStr = getRTValArgValue("type");
  RTVal path = getRTValArgValue("path");

  // Get the type not as a string, but as a KL Type.
  // Need to construct the strict/object from the 
  // string to then get the type.
  RTVal obj = RTVal::Construct(
    FabricApplicationStates::GetAppStates()->getContext(), 
    typeAsStr.getStringCString(), 
    0, 0);

  RTVal objType = obj.callMethod("Type", "type", 0, 0);

  RTVal args[3] = { 
    objType, 
    path,
    RTVal::ConstructBoolean(FabricApplicationStates::GetAppStates()->getContext(), true)
  };

  // Create the tool
  m_manager->getKLToolsManager().callMethod(
    "Tool::BaseTool",
    "createPathValueTool",
    3,
    args);

  // Update the tool'value from its target.
  m_manager->updateTool(
    path.getStringCString()
    );
 
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
