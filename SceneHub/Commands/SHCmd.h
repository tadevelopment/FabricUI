/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_H__
#define __UI_SCENEHUB_CMD_H__

#include "SHCmdDescription.h"
#include <FabricUI/SceneHub/SHGLScene.h>

namespace FabricUI {
namespace SceneHub {

class SHCmd {
  
  public:
    SHCmd();

    ~SHCmd() {}

    virtual SHCmdDescription registerCommand();

    /// Gets the command
    virtual QString getFromRTVal(FabricCore::RTVal command);
        
    void setScene(SHGLScene *scene);

    /// Gets the command
    QString getCommand();
    
    /// Sets the command
    void setCommand(QString command);
    
    /// Does nothing (don't call the command in KL).
    void doit();
    
    /// Undoes the command.
    void undo();
    
    /// Redoes the command.
    void redo();


  protected:
    /// Extracts the parameters from the command string.
    /// \param command The command
    QStringList extractParams(QString command);

    void execute(QString command); 

    /// Checks if the command has been already applied.
    bool wasInvoked();

    /// Command states
    enum State {
      State_New,
      State_Done,
      State_Undone,
      State_Redone
    };

    State m_state;
    QString m_command;
    unsigned m_coreUndoCount;
    SHGLScene *m_shGLScene;
    SHCmdDescription m_desctiption;
};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_CMD_H__
