/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_H__
#define __UI_SCENEHUB_CMD_H__

#include <assert.h>
#include <FTL/JSONEnc.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>
#include <FabricCore.h>
#include <QString>
#include <QMetaType>

namespace FabricUI {
namespace SceneHub {

class SHCmd {

  /**
    SHCmd is the base class to wrap a KL SHCmd into a C++ command.
    It defines all the needed methods to be inserted in third-party stack (QUndoStack).
    
    By default, the command description (what its diplayed in the stack)
    is the command KL type.

    In order to specialize the command description, it's necessary to override this class 
    and register it as a QMetaType by implementing the registerCommand method.

    Any command should inherate from this class.
  */

  public:
    SHCmd();

    virtual ~SHCmd();

    /// Registers the command as a QMetaType.
    /// Must be implemented to override the default description.
    /// To override.
    virtual void registerCommand();

    /// Sets/creates the command from the KL command.
    /// Specializes the command description (what it's displayed in the stack).
    /// To override.
    virtual void setFromRTVal(FabricCore::Client client, FabricCore::RTVal cmd);
        
    /// Gets the command description.
    QString getDescription();

    /// Does nothing (don't call the command in KL).
    void doit();
    
    /// Undoes the command.
    void undo();
    
    /// Redoes the command.
    void redo();


  protected:
    /// \internal
    /// Command states
    enum State {
      State_New,
      State_Done,
      State_Undone,
      State_Redone
    };

    /// Command state.
    State m_state;
    /// Command description (what it's diplayed in the stack).
    QString m_description;
    /// Number of undo/redo to perform.
    unsigned m_coreUndoCount;
    /// Reference to the FabricCore::Client.
    FabricCore::Client m_client;
};

} // SceneHub
} // FabricUI

#endif // __UI_SCENEHUB_CMD_H__
