/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_H__
#define __UI_SCENEHUB_CMD_H__

#include <FabricUI/SceneHub/SHGLScene.h>

namespace FabricUI {
namespace SceneHub {


class SHCmd {
  
  public:
    /// Constructors.
    SHCmd();

    virtual ~SHCmd() {}

    /// Registers the commands.
    /// Must be called so that the commands appears within the undo stack.
    /// To override.
    virtual void registerCommand() {}

    /// Sets/creates the command from the commands RTVal.
    /// To override.
    virtual void setFromRTVal(FabricCore::Client client, FabricCore::RTVal command) {}
        
    /// Gets the command description.
    QString getDescription();

    /// Executes the command the first time.
    /// Does nothing (the command is executed from KL).
    void doit();
    
    /// Undoes the command.
    void undo();
    
    /// Redoes the command.
    void redo();


  protected:
    /// Command states
    enum State {
      State_New,
      State_Done,
      State_Undone,
      State_Redone
    };

    State m_state;
    QString m_description;
    unsigned m_coreUndoCount;
    FabricCore::Client m_client;
};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_CMD_H__
