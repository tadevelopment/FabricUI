//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "../OptionsEditorHelpers.h"
#include "SetKLOptionsTargetModelItemCommand.h"
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Commands/CommandArgHelpers.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace OptionsEditor;

SetKLOptionsTargetModelItemCommand::SetKLOptionsTargetModelItemCommand() 
  : BaseSetRTValModelItemCommand()
{
}

SetKLOptionsTargetModelItemCommand::~SetKLOptionsTargetModelItemCommand()
{
}

void SetKLOptionsTargetModelItemCommand::setSingleOption(
  const QString &editorID,
  const QString &optionsPath,
  RTVal value) 
{
  KLCommandManager *manager = dynamic_cast<KLCommandManager*>(
    CommandManager::GetCommandManager());

  SetKLOptionsTargetSingleOption(
    manager->getClient(), 
    editorID,
    optionsPath,
    value);
}

RTVal SetKLOptionsTargetModelItemCommand::getOptions(
  const QString &editorID) 
{
  KLCommandManager *manager = dynamic_cast<KLCommandManager*>(
    CommandManager::GetCommandManager());

  return GetKLOptionsTargetOptions(
    manager->getClient(),
    editorID);
}

QString SetKLOptionsTargetModelItemCommand::getHelp()
{
   QMap<QString, QString> argsHelp;

  argsHelp["editorID"] = "Qt objectName of the option editor / ID of the KL option in the OptionsTargetRegistry";
  argsHelp["optionsPath"] = "Path of the option in the editor model hierarchy";
  argsHelp["newValue"] = "New option";
  argsHelp["previousValue"] = "Previous option";

  return CreateHelpFromRTValArgs(
    "Sets the value of a KL OptionsEditorTarget model item",
    argsHelp,
    this);
}
