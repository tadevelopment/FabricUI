//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "CreateToolCommand.h"
#include <FabricUI/Util/QtUtil.h>
#include <FabricUI/Commands/CommandHelpers.h>
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace Tools;
using namespace Commands;
using namespace Application;

CreateToolCommand::CreateToolCommand() 
  : BaseRTValScriptableCommand()
{
  FABRIC_CATCH_BEGIN();

  declareRTValArg(
    "toolType", 
    "String");
  
  // // No-optional arg of unknown KL type, which
  // // is retrieved when executing the command.
  // declareRTValArg(
  //   "newValue",
  //   "RTVal",
  //   CommandArgFlags::IN_ARG);


  // declareRTValArg("toolType", "String", "");
  // //declareArg("text", false, "");

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

bool CreateToolCommand::doIt()
{
  FABRIC_CATCH_BEGIN();

  // QMainWindow *mainWindow = QtUtil::getMainWindow();

  // SimpleDialog *dialog = new SimpleDialog(
  //   getArg("title"), 
  //   getArg("text"), 
  //   mainWindow);

  // QPoint pos = QCursor::pos();
  // dialog->move(pos.x(), pos.y());

  // dialog->show();
  return true;

  FABRIC_CATCH_END("CreateToolCommand::doIt");

  return false;
}

QString CreateToolCommand::getHelp()
{
  FABRIC_CATCH_BEGIN();

  QMap<QString, QString> argsHelp;
  argsHelp["toolType"] = "Type of the tool (KL)";
 
  return CommandHelpers::createHelpFromRTValArgs(
    this,
    "Create a new tool",
    argsHelp);

  FABRIC_CATCH_END("CreateToolCommand::getHelp");

  return "";
}
