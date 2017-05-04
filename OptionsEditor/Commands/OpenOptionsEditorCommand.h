//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_OPEN_OPTIONS_EDITOR_COMMAND__
#define __UI_OPEN_OPTIONS_EDITOR_COMMAND__

#include "BaseOptionsEditorCommand.h"
 
namespace FabricUI {
namespace OptionsEditor {

class OpenOptionsEditorCommand : public BaseOptionsEditorCommand
{
  /**
    Command to open an option editor widget. 

    Arguments:
      - registryID (String): ID of the KL OptionsEditorTarget in the registry
      - editorTitle (String): Title of the widget
  */

  public:
    OpenOptionsEditorCommand();

    virtual ~OpenOptionsEditorCommand();

    /// Implementation of Commands::BaseCommand.
    virtual bool doIt();

    /// Implementation of Commands::BaseCommand.
    virtual QString getHelp();
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_OPEN_OPTIONS_EDITOR_COMMAND__
