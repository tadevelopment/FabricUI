// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_BASE_RTVAL_PATH_VALUE_COMMAND__
#define __UI_BASE_RTVAL_PATH_VALUE_COMMAND__

#include "BaseRTValScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class BaseRTValPathValueCommand : public BaseRTValScriptableCommand
{
  Q_OBJECT
  
  public:
    BaseRTValPathValueCommand();

    virtual ~BaseRTValPathValueCommand();

    /// Implementation of BaseCommand.
    virtual bool canUndo();

    /// Implementation of BaseCommand.
    virtual bool doIt();

    /// Implementation of BaseCommand.
    virtual bool undoIt();

     /// Implementation of BaseCommand.
    virtual bool redoIt();

    /// Implementation of BaseCommand.
    virtual QString getHistoryDesc();
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_RTVAL_PATH_VALUE_COMMAND__
