//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <FabricCore.h>
#include "../OptionsEditorHelpers.h"
#include <FabricUI/Util/FabricException.h>
#include "CloseKLOptionsTargetEditorCommand.h"
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Commands/CommandArgHelpers.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace OptionsEditor;

CloseKLOptionsTargetEditorCommand::CloseKLOptionsTargetEditorCommand() 
  : BaseRTValScriptableCommand()
  , m_canLog(true)
{
  try
  {
    KLCommandManager *manager = dynamic_cast<KLCommandManager *>(
      CommandManager::GetCommandManager());
    
    declareRTValArg("editorID", "String");

    declareRTValArg(
      "failSilently",
      "Boolean",
      CommandArgFlags::OPTIONAL_ARG | CommandArgFlags::LOGGABLE_ARG,
      RTVal::ConstructBoolean(
        manager->getClient(), 
        false)
      );
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "CloseKLOptionsTargetEditorCommand::CloseKLOptionsTargetEditorCommand",
      "",
      e.what());
  }
};

CloseKLOptionsTargetEditorCommand::~CloseKLOptionsTargetEditorCommand()
{
}

bool CloseKLOptionsTargetEditorCommand::canUndo() {
  return false;
}

bool CloseKLOptionsTargetEditorCommand::canLog() {
  return m_canLog;
}

bool CloseKLOptionsTargetEditorCommand::doIt() 
{ 
  bool res = false;

  try
  {
    bool failSilently = getRTValArg("failSilently").getBoolean();
    QString editorID = getRTValArg("editorID").getStringCString();

    QWidget *dock = GetOptionsEditorDock(editorID);

    if(dock == 0)
    {
      m_canLog = false;
      res = failSilently;
    }

    else
    {
      dock->close();
      res = true;
    }
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "CloseKLOptionsTargetEditorCommand::doIt",
      "",
      e.what());
  }

  return res;
}

QString CloseKLOptionsTargetEditorCommand::getHelp()
{
  QMap<QString, QString> argsHelp;

  argsHelp["editorID"] = "Qt objectName of the option editor / ID of the KL option in the OptionsTargetRegistry";
  argsHelp["failSilently"] = "If false, throws an error if the widget has not been closed";

  return CreateHelpFromRTValArgs(
    "Close a Qt editor to edit a KL OptionsTarget",
    argsHelp,
    this);
}

QString CloseKLOptionsTargetEditorCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;

  argsDesc["editorID"] = getRTValArg(
    "editorID").getStringCString();

  argsDesc["failSilently"] = QString::number(
    getRTValArg("failSilently").getBoolean()
    );
 
  return CreateHistoryDescFromArgs(
    argsDesc,
    this);
}
