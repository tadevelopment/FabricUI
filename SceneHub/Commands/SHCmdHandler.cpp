/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHCmdHandler.h"
   
using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;


class SHCmdHandler::WrappedCmd : public QUndoCommand {
  public:
    WrappedCmd(SHCmd *shCmd) 
      : QUndoCommand()
      , m_shCmd( shCmd )
      , m_didit( false ) {}

  protected:
    virtual void redo() 
    {
      try 
      {
        if(m_didit) m_shCmd->redo();
        else 
        {
          m_didit = true;
          m_shCmd->doit();
        }
        QUndoCommand::setText( m_shCmd->getDescription() );
      }
      catch (Exception e) {
        printf("SHCmdHandler::WrappedCmd error: %s\n", e.getDesc_cstr() );
      }
    }

    virtual void undo() {
      try 
      {
        assert(m_didit);
        m_shCmd->undo();
      }
      catch (Exception e) {
        printf("SHCmdHandler::WrappedCmd error: %s\n", e.getDesc_cstr() );
      }
    }

    FTL::OwnedPtr<SHCmd> m_shCmd;
    bool m_didit;
};

SHCmdHandler::SHCmdHandler(Client client, QUndoStack *qUndoStack)
  : m_stackSize(0)
  , m_client(client)
  , m_qUndoStack(qUndoStack) {
}

void SHCmdHandler::onSynchronizeCommands() {
  try 
  {
    RTVal cmdManager = RTVal::Create(m_client, "CmdManager", 0, 0);
    cmdManager = cmdManager.callMethod("CmdManager", "getOrCreateCmdManager", 0, 0);

    unsigned int currentStackSize = cmdManager.callMethod("Size", "getNumCmdInUndoStack", 0, 0).getUInt32();

    // Get the number of commands already done in the KL stack
    for(unsigned int i=m_stackSize; i<currentStackSize; ++i)
    {        
      RTVal indexVal = RTVal::ConstructUInt32(m_client, i);
      RTVal cmdVal = cmdManager.callMethod("Cmd", "getCmdInUndoStack", 1, &indexVal);

      QString type = QString(cmdVal.callMethod("String", "type", 0, 0).getStringCString());

      int id = QMetaType::type(type.toUtf8().constData());
      if(id != 0) 
      {
        SHCmd *cmd = static_cast <SHCmd*>(QMetaType::construct(id));
        cmd->setFromRTVal(m_client, cmdVal);
        m_qUndoStack->push(new WrappedCmd(cmd));
      }
    }

    m_stackSize = currentStackSize;
  }
  catch(Exception e)
  {
    printf("SHCmdHandler::onSynchronizeCommands: exception: %s\n", e.getDesc_cstr());
  }
}
