//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "../OptionsEditorHelpers.h"
#include <FabricUI/Util/RTValUtil.h>
#include "BaseSetRTValModelItemCommand.h"
#include <FabricUI/Commands/CommandException.h>
#include <FabricUI/Commands/KLCommandManager.h>

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
    declareRTValArg(
      "editorID",
      "String",
      false);

    declareRTValArg(
      "optionsPath", 
      "String", 
      false);
    
    // Declares an optional argument of 
    // unknowned type which is not loggable.
    declareArg(
      "previousValue", 
      true,
      "",
      false);

    // Declares an no-optional 
    // arg of unknowned type.
    declareArg(
      "newValue", 
      false);
  }

  catch(Exception &e)
  {
    CommandException::PrintOrThrow(
      "BaseSetRTValModelItemCommand::BaseSetRTValModelItemCommand",
      "",  
      e.getDesc_cstr(),
      PRINT | THROW);
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "BaseSetRTValModelItemCommand::BaseSetRTValModelItemCommand",
      "",
      e.what(),
      PRINT | THROW);
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

  catch(Exception &e)
  {
    CommandException::PrintOrThrow(
      "BaseSetRTValModelItemCommand::exeCommand",
      "",  
      e.getDesc_cstr(),
      PRINT | THROW);
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "BaseSetRTValModelItemCommand::exeCommand",
      "",
      e.what(),
      PRINT | THROW);
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

  catch(Exception &e)
  {
    CommandException::PrintOrThrow(
      "BaseSetRTValModelItemCommand::checkArguments",
      "",  
      e.getDesc_cstr(),
      PRINT | THROW);
  }

  catch(CommandException &e) 
  {
    CommandException::PrintOrThrow(
      "BaseSetRTValModelItemCommand::checkArguments",
      "",
      e.what(),
      PRINT | THROW);
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

  return createHistoryDescFromArgs(
    argsDesc);
}

