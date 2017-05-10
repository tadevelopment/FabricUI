//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SET_KL_OPTIONS_TARGET_MODEL_ITEM_COMMAND__
#define __UI_SET_KL_OPTIONS_TARGET_MODEL_ITEM_COMMAND__

#include "BaseSetRTValModelItemCommand.h"

namespace FabricUI {
namespace OptionsEditor {

class SetKLOptionsTargetModelItemCommand : public BaseSetRTValModelItemCommand
{
  /**
    SetKLOptionsTargetModelItemCommand sets the value of a single 
    KLOptionsTargetModelItem.
    
    Arguments:
    - editorID (String): Qt objectName of the option editor / ID of the KL option in the OptionsTargetRegistry
    - optionsPath (String): Path of the option in the editor model hierarchy
    - newValue (RTVal): New option
    - previousValue (RTVal): Previous option
  */

  public:
    SetKLOptionsTargetModelItemCommand();

    virtual ~SetKLOptionsTargetModelItemCommand();
 
    /// Implementation of Command.
    virtual QString getHelp();
    
  protected:
    /// Implementation of BaseSetRTValModelItemCommand.
    virtual void setSingleOption(
      const QString &editorID,
      const QString &optionsPath,
      FabricCore::RTVal singleOption
      );

    /// Implementation of BaseSetRTValModelItemCommand.
    virtual FabricCore::RTVal getOptions(
      const QString &editorID
      );
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_SET_KL_OPTIONS_TARGET_MODEL_ITEM_COMMAND__
