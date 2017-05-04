//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include "../RTValItem.h"
#include "../OptionsEditorHelpers.h"
#include <FabricUI/Util/RTValUtil.h>
#include "SetOptionsModelValueCommand.h"
#include <FabricUI/Commands/KLCommandRegistry.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace OptionsEditor;

SetOptionsModelValueCommand::SetOptionsModelValueCommand() 
  : BaseOptionsEditorCommand()
{
  try
  {
    declareRTValArg(
      "optionsPath",
      "String",
      false);

    declareArg(
      "previousValue",
      false);

    declareArg(
      "newValue",
      false);
  }

  catch (std::string &e) 
  {
    printf(
      "SetOptionsModelValueCommand::SetOptionsModelValueCommand: exception: %s\n", 
      e.c_str());
  }

  catch(Exception &e)
  {
    printf(
      "SetOptionsModelValueCommand::SetOptionsModelValueCommand: exception: %s\n", 
      e.getDesc_cstr());
  }
};

SetOptionsModelValueCommand::~SetOptionsModelValueCommand()
{
}

bool SetOptionsModelValueCommand::executeCommand(
  RTVal value) 
{
  bool res = false;

  try
  {
    QString registryID = getRTValArg(
      "registryID"
      ).getStringCString();

    QString optionsPath = getRTValArg(
      "optionsPath"
      ).getStringCString();

    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
      CommandRegistry::GetCommandRegistry());

    SetKLOptionsTargetSingleOption(
      registry->getClient(), 
      registryID,
      optionsPath,
      value);
    
    BaseOptionsEditor *editor = GetOptionsEditor(
      registryID);

    if(editor)
      editor->updateModel();

    res = true;
  }

  catch(Exception &e)
  {
    printf(
      "SetOptionsModelValueCommand::executeCommand: exception: %s\n", 
      e.getDesc_cstr());
  }

  return res;
}

bool SetOptionsModelValueCommand::doIt() 
{ 
  RTVal val = getRTValArg("newValue");
  return executeCommand(val);
}

bool SetOptionsModelValueCommand::undoIt() 
{ 
  RTVal val = getRTValArg("previousValue");
  return executeCommand(val);
}

bool SetOptionsModelValueCommand::redoIt() 
{ 
  return doIt();
}

bool SetOptionsModelValueCommand::canUndo() 
{
  return true;
}

QString SetOptionsModelValueCommand::getHelp()
{
  QString help = "Sets the value of an OptionModel (internal).\n";
  help +=  "Arguments:\n";
  help +=  "- optionModelAdress (String): Adress of the OptionModel's pointer\n";
  help +=  "- previousValue (RTVal): Previous model value\n";
  help +=  "- newValue (RTVal): New model value";
  return help;
}
 