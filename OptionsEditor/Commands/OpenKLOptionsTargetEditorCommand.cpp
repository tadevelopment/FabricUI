//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricCore.h>
#include "../OptionsEditorHelpers.h"
#include "OpenKLOptionsTargetEditorCommand.h"
#include <FabricUI/Commands/CommandException.h>
#include <FabricUI/Commands/KLCommandManager.h>

using namespace FabricUI;
using namespace OptionsEditor;
using namespace FabricCore;
using namespace Commands;

OpenKLOptionsTargetEditorCommand::OpenKLOptionsTargetEditorCommand() 
  : BaseRTValScriptableCommand()
{
  try
  {
    declareRTValArg(
      "editorID",
      "String",
      false);

    declareRTValArg(
      "editorTitle",
      "String",
      false);

    declareRTValArg(
      "groupName",
      "String",
      true);
  }

  catch(Exception &e)
  {
    CommandException::PrintOrThrow(
      "OpenKLOptionsTargetEditorCommand::OpenKLOptionsTargetEditorCommand",
      "",  
      e.getDesc_cstr(),
      PRINT | THROW);
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "OpenKLOptionsTargetEditorCommand::OpenKLOptionsTargetEditorCommand",
      "",
      e.what(),
      PRINT | THROW);
  }
};

OpenKLOptionsTargetEditorCommand::~OpenKLOptionsTargetEditorCommand()
{
}

bool OpenKLOptionsTargetEditorCommand::canUndo() {
  return false;
}

/// Implementation of Command.
bool OpenKLOptionsTargetEditorCommand::doIt() 
{ 
  bool res = false;

  try
  {
    QString editorID = getRTValArg(
      "editorID"
      ).getStringCString();

    QWidget *dock = GetOptionsEditorDock(editorID);
    if(dock != 0)
    {
      if(dock->isHidden())
        dock->show();
    }

    else
    {
      QString editorTitle = getRTValArg(
        "editorTitle"
        ).getStringCString();

      QString groupName = isArgSet("groupName")
        ? getRTValArg("groupName").getStringCString()
        : QString();

      KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
        CommandManager::GetCommandManager());
     
      CreateOptionsEditor(
        manager->getClient(), 
        editorID,
        editorTitle,
        groupName);
    }

    res = true;
  }

  catch(Exception &e)
  {
    CommandException::PrintOrThrow(
      "OpenKLOptionsTargetEditorCommand::doIt",
      "",
      e.getDesc_cstr(),
      PRINT | THROW);
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "OpenKLOptionsTargetEditorCommand::doIt",
      "",
      e.what(),
      PRINT | THROW);
  }

  return res;
}

QString OpenKLOptionsTargetEditorCommand::getHelp()
{
  QMap<QString, QString> argsHelp;

  argsHelp["editorID"] = "Qt objectName of the option editor / ID of the KL option in the OptionsTargetRegistry";
  argsHelp["editorTitle"] = "Title of the widget";
  argsHelp["groupName"] = "Name of the options' group";
  
  return createHelpFromArgs(
    "Open a Qt editor to edit a KL OptionsTarget",
    argsHelp);
}

QString OpenKLOptionsTargetEditorCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;

  argsDesc["editorID"] = getRTValArg(
    "editorID").getStringCString();

  argsDesc["editorTitle"] = getRTValArg(
    "editorTitle").getStringCString();

  return createHistoryDescFromArgs(
    argsDesc);
}
