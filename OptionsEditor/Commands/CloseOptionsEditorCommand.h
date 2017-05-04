//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_CLOSE_OPTIONS_EDITOR_COMMAND__
#define __UI_CLOSE_OPTIONS_EDITOR_COMMAND__

#include "BaseOptionsEditorCommand.h"
 
namespace FabricUI {
namespace OptionsEditor {

class CloseOptionsEditorCommand : public BaseOptionsEditorCommand
{
  /**
    Command to close an option editor widget. 

    Arguments:
      - registryID (String): ID of the KL OptionsEditorTarget in the registry
      - failSilently (Boolean): If false, throws an error if the widget has not been closed
  */

  public:
    CloseOptionsEditorCommand();

    virtual ~CloseOptionsEditorCommand();

    /// Implementation of Commands::BaseCommand.
    virtual bool doIt();

    /// Implementation of Commands::BaseCommand.
    virtual QString getHelp();
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_CLOSE_OPTIONS_EDITOR_COMMAND__
