//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_OPTIONS_EDITOR_COMMAND__
#define __UI_BASE_OPTIONS_EDITOR_COMMAND__

#include <FabricUI/Commands/BaseRTValScriptableCommand.h>

namespace FabricUI {
namespace OptionsEditor {

class BaseOptionsEditorCommand : public Commands::BaseRTValScriptableCommand
{
  /**
    BaseOptionsEditorCommand defines a base command for interacting 
    with.
    BaseOptionsEditorCommand commands should specialized it.
  */  
    
  public:
    BaseOptionsEditorCommand();

    virtual ~BaseOptionsEditorCommand();

    /// Implementation of Commands::BaseCommand.
    virtual bool canUndo();
};

} // namespace OptionsEditor 
} // namespace FabricUI

#endif // __UI_BASE_OPTIONS_EDITOR_COMMAND__
