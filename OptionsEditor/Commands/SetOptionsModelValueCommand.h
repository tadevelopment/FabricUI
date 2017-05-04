//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SET_MODEL_VALUE_COMMAND__
#define __UI_SET_MODEL_VALUE_COMMAND__

#include "BaseOptionsEditorCommand.h"

namespace FabricUI {
namespace OptionsEditor {

class SetOptionsModelValueCommand : public BaseOptionsEditorCommand
{
  /**
    Command to sets the value of an OptionModel (internal).
    
    Arguments:
    - optionModelAdress (String): Adress of the OptionModel's pointer
    - previousValue (RTVal): Previous model value
    - newValue (RTVal): New model value
  */

  public:
    SetOptionsModelValueCommand();

    virtual ~SetOptionsModelValueCommand();
   
    /// Implementation of Commands::BaseCommand.
    virtual bool doIt();

    /// Implementation of Commands::BaseCommand.
    virtual bool undoIt();

    /// Implementation of Commands::BaseCommand.
    virtual bool redoIt();

    /// Implementation of Commands::BaseCommand.
    virtual bool canUndo();

    /// Implementation of Commands::BaseCommand.
    virtual QString getHelp();

  private:
    bool executeCommand(
      FabricCore::RTVal val);

};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_SET_MODEL_VALUE_COMMAND__
