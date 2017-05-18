//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "../OptionsEditorHelpers.h"
#include <FabricUI/Util/RTValUtil.h>
#include "BaseSetRTValModelItemCommand.h"
#include <FabricUI/Util/FabricException.h>
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/Commands/CommandArgHelpers.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace OptionsEditor;

BaseSetRTValModelItemCommand::BaseSetRTValModelItemCommand() 
  : BaseRTValScriptableCommand()
{
  try
  {
    declareRTValArg("editorID","String");
    declareRTValArg("optionsPath", "String");
    
    // Declares an optional argument of 
    // unknowned type which is not loggable.
    declareArg("previousValue", CommandArgFlags::OPTIONAL_ARG);

    // Declares an no-optional 
    // arg of unknowned type.
    declareArg("newValue");
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseSetRTValModelItemCommand::BaseSetRTValModelItemCommand",
      "",
      e.what());
  }
};

BaseSetRTValModelItemCommand::~BaseSetRTValModelItemCommand()
{
}

bool BaseSetRTValModelItemCommand::exeCommand(
  const QString &strValue) 
{
  bool res = false;

  try
  {
    QString editorID = getRTValArg(
      "editorID"
      ).getStringCString();

    QString optionsPath = getRTValArg(
      "optionsPath"
      ).getStringCString();

    // Get the value from the arg.
    RTVal value = getRTValArg(strValue);

    setSingleOption(
      editorID,
      optionsPath,
      value);

    GetOptionsEditor(editorID)->updateModel();

    res = true;
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseSetRTValModelItemCommand::exeCommand",
      "",
      e.what());
  }

  return res;
}
 
void BaseSetRTValModelItemCommand::checkArguments()
{
  try
  {
    QString editorID = getRTValArg(
    "editorID"
    ).getStringCString();

    RTVal options = getOptions(editorID);

    QString optionsPath = getRTValArg(
      "optionsPath"
      ).getStringCString();

    KLCommandManager *manager = dynamic_cast<KLCommandManager*>(
      CommandManager::GetCommandManager());

    // Get the type of the option
    // Dig in the current options.
    RTVal singleOption = GetKLSingleOption(
      manager->getClient(),
      optionsPath,
      options);

    QString type = RTValUtil::getRTValType(
      singleOption);

    // Set the value from the arg.
    setRTValArgType("newValue", type);

    // The previous value arg is set.
    if(isArgSet("previousValue"))
      setRTValArgType("previousValue", type);

    // Otherwise, set the previous value  
    // equals to the current value
    else
      setRTValArg(
        "previousValue", 
        singleOption);
  }

  catch(FabricException &e) 
  {
    FabricException::Throw(
      "BaseSetRTValModelItemCommand::checkArguments",
      "",
      e.what());
  }
}

bool BaseSetRTValModelItemCommand::doIt() 
{ 
  checkArguments();
  return exeCommand("newValue");
}

bool BaseSetRTValModelItemCommand::undoIt() 
{ 
  return exeCommand("previousValue");
}

bool BaseSetRTValModelItemCommand::redoIt() 
{ 
  return exeCommand("newValue");
}

bool BaseSetRTValModelItemCommand::canUndo() 
{
  return true;
}

QString BaseSetRTValModelItemCommand::getHistoryDesc()
{
  QMap<QString, QString> argsDesc;

  argsDesc["editorID"] = getRTValArg(
    "editorID").getStringCString();

  argsDesc["optionsPath"] = getRTValArg(
    "optionsPath").getStringCString();

  return CreateHistoryDescFromArgs(
    argsDesc,
    this);
}

