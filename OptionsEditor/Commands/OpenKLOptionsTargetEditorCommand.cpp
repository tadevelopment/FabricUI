//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricCore.h>
#include "../OptionsEditorHelpers.h"
#include "OpenKLOptionsTargetEditorCommand.h"
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Commands/CommandArgHelpers.h>
#include <FabricUI/Application/FabricException.h>
 
using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace Application;
using namespace OptionsEditor;

OpenKLOptionsTargetEditorCommand::OpenKLOptionsTargetEditorCommand() 
  : BaseRTValScriptableCommand()
  , m_canLog(true)
{
  try
  {
    declareRTValArg("editorID", "String");
    
    declareRTValArg("editorTitle", "String");

    declareRTValArg(
      "groupName",
      "String",
      CommandArgFlags::OPTIONAL_ARG | CommandArgFlags::LOGGABLE_ARG);
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "OpenKLOptionsTargetEditorCommand::OpenKLOptionsTargetEditorCommand",
      "",
      e.what());
  }
};

OpenKLOptionsTargetEditorCommand::~OpenKLOptionsTargetEditorCommand()
{
}

bool OpenKLOptionsTargetEditorCommand::canUndo() 
{
  return false;
}

bool OpenKLOptionsTargetEditorCommand::canLog() 
{
  return m_canLog;
}

BaseRTValOptionsEditor* OpenKLOptionsTargetEditorCommand::getOptionsEditor() 
{
  QString editorID = getRTValArgValue(
    "editorID"
    ).getStringCString();

  return GetOptionsEditor(editorID);
}

QDockWidget* OpenKLOptionsTargetEditorCommand::getOptionsEditorDock() 
{
  QString editorID = getRTValArgValue(
    "editorID"
    ).getStringCString();

  return GetOptionsEditorDock(editorID);
}

bool OpenKLOptionsTargetEditorCommand::doIt() 
{ 
  bool res = false;

  try
  {
    QString editorID = getRTValArgValue(
      "editorID"
      ).getStringCString();

    QWidget *dock = GetOptionsEditorDock(editorID);
    if(dock != 0)
    {
      if(dock->isHidden())
        dock->show();
      else
        m_canLog = false;
    }

    else
    {
      QString editorTitle = getRTValArgValue(
        "editorTitle"
        ).getStringCString();

      QString groupName = isArgSet("groupName")
        ? getRTValArgValue("groupName").getStringCString()
        : QString();

      CreateOptionsEditor(
        editorID,
        editorTitle,
        groupName);
    }

    res = true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "OpenKLOptionsTargetEditorCommand::doIt",
      "",
      e.what());
  }

  return res;
}

QString OpenKLOptionsTargetEditorCommand::getHelp()
{
  QMap<QString, QString> argsHelp;

  argsHelp["editorID"] = "Qt objectName of the option editor / ID of the KL option in the OptionsTargetRegistry";
  argsHelp["editorTitle"] = "Title of the widget";
  argsHelp["groupName"] = "Name of the options' group";
  
  return CommandArgHelpers::CreateHelpFromRTValArgs(
    "Open a Qt editor to edit a KL OptionsTarget",
    argsHelp,
    this);
}

QString OpenKLOptionsTargetEditorCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;

  argsDesc["editorID"] = getRTValArgValue(
    "editorID").getStringCString();

  argsDesc["editorTitle"] = getRTValArgValue(
    "editorTitle").getStringCString();

  return CommandArgHelpers::CreateHistoryDescFromArgs(
    argsDesc,
    this);
}
