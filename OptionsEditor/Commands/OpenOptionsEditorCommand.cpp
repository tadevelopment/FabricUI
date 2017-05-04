//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <string>
#include <FabricCore.h>
#include "OpenOptionsEditorCommand.h"
#include <FabricUI/Commands/KLCommandRegistry.h>
#include "../OptionsEditorHelpers.h"

using namespace FabricUI;
using namespace OptionsEditor;
using namespace FabricCore;
using namespace Commands;

OpenOptionsEditorCommand::OpenOptionsEditorCommand() 
  : BaseOptionsEditorCommand()
{
  try
  {
    declareRTValArg(
      "editorTitle",
      "String",
      false);

    declareRTValArg(
      "groupName",
      "String",
      true);
  }

  catch(std::string &e) 
  {
    printf(
      "OpenOptionsEditorCommand::OpenOptionsEditorCommand: exception: %s\n", 
      e.c_str());
  }

  catch(Exception &e)
  {
    printf(
      "OpenOptionsEditorCommand::OpenOptionsEditorCommand: exception: %s\n", 
      e.getDesc_cstr());
  }
};

OpenOptionsEditorCommand::~OpenOptionsEditorCommand()
{
}

/// Implementation of Commands::BaseCommand.
bool OpenOptionsEditorCommand::doIt() 
{ 
  bool res = false;

  try
  {
    QString registryID = getRTValArg(
      "registryID"
      ).getStringCString();

    QWidget *dock = GetOptionsEditorDock(registryID);

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

      RTVal groupNameVal = getRTValArg("groupName");
      QString groupName = groupNameVal.isValid()
        ? groupNameVal.getStringCString()
        : QString();

      KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
        CommandRegistry::GetCommandRegistry());
     
      CreateOptionsEditor(
        registry->getClient(), 
        registryID,
        editorTitle,
        groupName);
    }

    res = true;
  }

  catch (std::string &e) 
  {
    printf(
      "OpenOptionsEditorCommand::doIt: exception: %s\n", 
      e.c_str());
  }

  catch(Exception &e)
  {
    printf(
      "OpenOptionsEditorCommand::doIt: exception: %s\n", 
      e.getDesc_cstr());
  }

  return res;
}

/// Implementation of Commands::BaseCommand.
QString OpenOptionsEditorCommand::getHelp()
{
  QString help = "Open an option editor widget.\n";
  help +=  "Arguments:\n";
  help +=  "- registryID (String): ID of the KL OptionsEditorTarget in OptionsEditorTargetRegistry\n";
  help +=  "- editorTitle (String): Title of the widget";
  return help;
}
 