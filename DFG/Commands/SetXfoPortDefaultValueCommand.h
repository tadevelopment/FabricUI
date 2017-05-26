// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_SET_XFO_PORT_DEFAULT_VALUE_COMMAND__
#define __UI_SET_XFO_PORT_DEFAULT_VALUE_COMMAND__

#include <FabricUI/Commands/BaseRTValScriptableCommand.h>

namespace FabricUI {
namespace DFG {

class SetXfoPortDefaultValueCommand : public Commands::BaseRTValScriptableCommand
{
  Q_OBJECT
  
  public:    
    SetXfoPortDefaultValueCommand();

    virtual ~SetXfoPortDefaultValueCommand();

    /// Implementation of Command.
    virtual bool canUndo();

    /// Implementation of Command.
    virtual bool doIt();

    /// Implementation of Command.
    virtual bool undoIt();

     /// Implementation of Command.
    virtual bool redoIt();

    /// Implementation of Command.
    virtual QString getHelp();

    /// Implementation of Command.
    virtual QString getHistoryDesc();
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_SET_XFO_PORT_DEFAULT_VALUE_COMMAND__
