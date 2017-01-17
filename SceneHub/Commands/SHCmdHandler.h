/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_HANDLER_H__
#define __UI_SCENEHUB_CMD_HANDLER_H__

#include "SHCmd.h"
#include <QUndoStack>
 
namespace FabricUI {
namespace SceneHub {

class SHCmdHandler : public QObject {

  /**
    SHCmdHandler manages the SceneHub commands in the QUndoStack.
    
    When a KL command is generated, it's necessery to call the 
    onSynchronizeCommands method to synchronize the Qt stack and the KL stack.
  */

  Q_OBJECT
 
  public:
    /// Constructors.
    /// \param client A reference to the FabricCore::Client.
    /// \param qUndoStack A pointer to the Qt commands stacks.
    SHCmdHandler(FabricCore::Client client, QUndoStack *qUndoStack);
      
    virtual ~SHCmdHandler();

    /// Registers all the specific commands.
    /// Can be overriden if you want to register your own commands.
    virtual void registerCommands();


  public slots:
    /// Synchronizes the KL and the Qt stack.
    /// Must be called after adding a commands to the kl stack.
    void onSynchronizeCommands();


  protected:  
    /// Wrap a SHCmd into a QUndoCommand.
    class WrappedCmd;
     
    /// SceneHub number of commands (!= number in QUndoStack).
    unsigned int m_stackSize;
    /// Reference to the FabricCore Client.
    FabricCore::Client m_client;
    /// Pointor to the Qt stack.
    QUndoStack *m_qUndoStack;
};

} // SceneHub
} // FabricUI 

#endif // __UI_SCENEHUB_CMD_HANDLER_H__
