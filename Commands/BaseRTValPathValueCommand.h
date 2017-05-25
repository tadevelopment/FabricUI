// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_BASE_RTVAL_PATH_VALUE_COMMAND__
#define __UI_BASE_RTVAL_PATH_VALUE_COMMAND__

#include "BaseRTValScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class BaseRTValPathValueCommand : public BaseRTValScriptableCommand
{
  public:
    BaseRTValPathValueCommand();

    virtual ~BaseRTValPathValueCommand();

    /// Implementation of Command.
    virtual bool canUndo();

    /// Implementation of Command.
    virtual bool doIt();

    /// Implementation of Command.
    virtual bool undoIt();

     /// Implementation of Command.
    virtual bool redoIt();

    /// Implementation of Command.
    virtual QString getHistoryDesc();
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_RTVAL_PATH_VALUE_COMMAND__
