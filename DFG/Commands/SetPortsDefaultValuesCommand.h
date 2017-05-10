// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_SETPORTSDEFAULTVALUES_COMMAND__
#define __UI_SETPORTSDEFAULTVALUES_COMMAND__

#include "BaseDFGCommand.h"

namespace FabricUI {
namespace DFG {

class SetPortsDefaultValuesCommand : public BaseDFGCommand 
{
  public:
    /**
      Args:
      - execPath (String): Absolute path of the DFGExec.
      - nodeName (String): Name of the node owning the port.
      - portName (String): Name of the port.
      - portValue (RTVal): Value to set, must be of the same that the port type.
      - isUndoable (Boolean): If true, the command is undoable.
    */
    
    SetPortsDefaultValuesCommand();

    virtual ~SetPortsDefaultValuesCommand();

    /// Implementation of Command.
    bool canUndo();
 
    /// Implementation of Command.
    virtual bool doIt();

    /// Implementation of Command.
    virtual QString getHelp();
    
    /// Implementation of Command.
    virtual QString getHistoryDesc();
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_SETPORTSDEFAULTVALUES_COMMAND__
