//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SET_RTVAL_MODEL_ITEM_COMMAND__
#define __UI_BASE_SET_RTVAL_MODEL_ITEM_COMMAND__

#include <FabricUI/Commands/BaseRTValScriptableCommand.h>

namespace FabricUI {
namespace OptionsEditor {

class BaseSetRTValModelItemCommand : public Commands::BaseRTValScriptableCommand
{
  /**
    BaseSetRTValModelItemCommand sets the value of a single RTVal-based OptionModelItem.
    
    Arguments:
    - editorID (String): ID (Qt objectName) of the option editor
    - optionsPath (String): Path of the option in the editor model hierarchy
    - newValue (Unknown): New option
    - previousValue (Unknown, optional): Previous option
  */

  public:
    BaseSetRTValModelItemCommand();

    virtual ~BaseSetRTValModelItemCommand();
   
    /// Implementation of Command.
    virtual bool doIt();

    /// Implementation of Command.
    virtual bool undoIt();

    /// Implementation of Command.
    virtual bool redoIt();

    /// Implementation of Command.
    virtual bool canUndo();

    /// Implementation of Command.
    virtual QString getHistoryDesc();
    
  protected:
    /// Sets a single option.
    /// To override
    virtual void setSingleOption(
      const QString &editorID,
      const QString &optionsPath,
      FabricCore::RTVal singleOption
      ) = 0;

    /// Gets all the options.
    /// To override
    virtual FabricCore::RTVal getOptions(
      const QString &editorID
      ) = 0;

  private:
    /// \internal 
    /// Executes the command, undo or redo.
    bool exeCommand(
      const QString &strValue
      );

    /// Checks the (newValue, previousValue) argument.
    /// Sets their type if needed -> support script
    void checkArguments();
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_BASE_SET_RTVAL_MODEL_ITEM_COMMAND__
