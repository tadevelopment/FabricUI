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
    SHCmd();

    virtual ~SHCmd() {}

    virtual void registerCommand() {}

    /// Gets the command
    virtual void setFromRTVal(FabricCore::Client client, FabricCore::RTVal command) {}
        
    /// Gets the command
    QString getDescription();

    /// Does nothing (don't call the command in KL).
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
