//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CreateToolCommand.h"
// #include <FabricUI/Util/QtUtil.h>
// #include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>
//#include <FabricUI/Application/FabricApplicationStates.h>
//#include <FabricUI/Commands/PathValueResolverRegistry.h>

using namespace FabricUI;
//using namespace Util;
using namespace Tools;
using namespace Commands;
using namespace FabricCore;
using namespace Application;

CreateToolCommand::CreateToolCommand() 
  : BaseRTValScriptableCommand()
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg("target", "RTVal",
    CommandArgFlags::IO_ARG);

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

  RTVal target = getRTValArg("target");

  m_manager->getKLToolsManager().callMethod(
    "Tool::BaseTool",
    "createPathValueTool",
    1,
    &target);

  // Update the tool'value from its target.
  m_manager->updateTool(target);
 
  return true;

  FABRIC_CATCH_END("CreateToolCommand::doIt");

  return false;
}

QString CreateToolCommand::getHelp()
{
  FABRIC_CATCH_BEGIN();

  QMap<QString, QString> argsHelp;
  argsHelp["target"] = "Tool arguments";

  return CommandHelpers::createHelpFromRTValArgs(
    this,
    "Create a new tool",
    argsHelp);

  FABRIC_CATCH_END("CreateToolCommand::getHelp");

  return "";
}
