//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//
 
#include <string>
#include <FabricCore.h>
#include "CloseOptionsEditorCommand.h"
#include <FabricUI/Commands/KLCommandRegistry.h>
#include "../OptionsEditorHelpers.h"

using namespace FabricUI;
using namespace OptionsEditor;
using namespace FabricCore;
using namespace Commands;

CloseOptionsEditorCommand::CloseOptionsEditorCommand() 
  : BaseOptionsEditorCommand()
{
  try
  {
    KLCommandRegistry *registry = dynamic_cast<KLCommandRegistry *>(
      CommandRegistry::GetCommandRegistry());
    
    Client client = registry->getClient();

    RTVal defaulVal = RTVal::ConstructBoolean(
      client, 
      false);

    declareRTValArg(
      "failSilently",
      "Boolean",
      false,
      defaulVal);
  }

  catch (std::string &e) 
  {
    printf(
      "CloseOptionsEditorCommand::CloseOptionsEditorCommand: exception: %s\n", 
      e.c_str());
  }

  catch(Exception &e)
  {
    printf(
      "CloseOptionsEditorCommand::CloseOptionsEditorCommand: exception: %s\n", 
      e.getDesc_cstr());
  }
};

CloseOptionsEditorCommand::~CloseOptionsEditorCommand()
{
}

bool CloseOptionsEditorCommand::doIt() 
{ 
  bool res = false;

  try
  {
    bool failSilently = getRTValArg(
      "failSilently"
      ).getBoolean();

    QString registryID = getRTValArg(
        "registryID"
        ).getStringCString();

    QWidget *dock = GetOptionsEditorDock(registryID);

    if(dock == 0)
      res = failSilently;
    else
    {
      dock->close();
      res = true;
    }
  }

  catch (std::string &e) 
  {
    printf(
      "CloseOptionsEditorCommand::doIt: exception: %s\n", 
      e.c_str());
  }

  catch(Exception &e)
  {
    printf(
      "CloseOptionsEditorCommand::doIt: exception: %s\n", 
      e.getDesc_cstr());
  }

  return res;
}

QString CloseOptionsEditorCommand::getHelp()
{
  QString help = "Close an option editor widget.\n";
  help +=  "Arguments:\n";
  help +=  "- registryID (String): ID of the KL OptionsEditorTarget in the registry\n";
  help +=  "- failSilently (Boolean): If false, throws an error if the widget has not been closed";
  return help;
}
