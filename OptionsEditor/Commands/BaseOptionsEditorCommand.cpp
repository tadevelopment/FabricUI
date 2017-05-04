//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricCore.h>
#include "BaseOptionsEditorCommand.h"
 
using namespace FabricUI;
using namespace OptionsEditor;
using namespace FabricCore;

BaseOptionsEditorCommand::BaseOptionsEditorCommand()
  : BaseRTValScriptableCommand()
{
  try
  {
    declareRTValArg(
      "registryID",
      "String",
      false);
  }

  catch (std::string &e) 
  {
    printf(
      "BaseOptionsEditorCommand::BaseOptionsEditorCommand: exception: %s\n", 
      e.c_str());
  }

  catch(Exception &e)
  {
    printf(
      "BaseOptionsEditorCommand::BaseOptionsEditorCommand: exception: %s\n", 
      e.getDesc_cstr());
  }
}

BaseOptionsEditorCommand::~BaseOptionsEditorCommand() 
{
}

bool BaseOptionsEditorCommand::canUndo() {
  return false;
}

