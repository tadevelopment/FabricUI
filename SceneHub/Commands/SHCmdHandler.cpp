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
        QUndoCommand::setText( m_shCmd->getCommand() );
      }
      catch (FabricCore::Exception e) {
        printf("SHCmdHandler::WrappedCmd error: %s\n", e.getDesc_cstr() );
      }
    }

    virtual void undo() {
      try 
      {
        assert(m_didit);
        m_shCmd->undo();
      }
      catch (FabricCore::Exception e) {
        printf("SHCmdHandler::WrappedCmd error: %s\n", e.getDesc_cstr() );
      }
    }

    FTL::OwnedPtr<SHCmd> m_shCmd;
    bool m_didit;
};

SHCmdHandler::SHCmdHandler(
  SHGLScene *scene, 
  SHCmdRegistration *cmdRegistration,
  QUndoStack *qUndoStack)
  : m_stackSize(0)
  , m_shGLScene(scene)
  , m_shCmdRegistration(cmdRegistration)
  , m_qUndoStack(qUndoStack) {
}

QString inline ExtractCommandName(QString command) {
  QString name;
  QStringList split = command.split("(");
  if(split.size() > 1) name = split[0];
  return name;
}

void SHCmdHandler::addCommand(QString command) {
 
  QString name = ExtractCommandName(command).toLower();
  
  QSetIterator<SHCmdDescription> ite(m_shCmdRegistration->getCmdDescriptionSet());
  while(ite.hasNext())
  {
    SHCmdDescription cmdDescription = ite.next();
    if(name == cmdDescription.cmdName.toLower()) 
    {
      int id = QMetaType::type(cmdDescription.cmdType.toUtf8().constData());
      if(id != 0) 
      {
        SHCmd *cmd = static_cast <SHCmd*>(QMetaType::construct(id));
        cmd->setScene(m_shGLScene);
        cmd->setCommand(command);
        m_qUndoStack->push(new WrappedCmd(cmd));
        break;
      }
    }
  }
}

void SHCmdHandler::onAddCommands() {

  // Get the number of commands already done in the KL stack
  for(unsigned int i=m_stackSize; i<m_shGLScene->getNumCmdInUndoStack(); ++i)
  {        
    FabricCore::RTVal sgCmd = m_shGLScene->retrieveCmd(i);
    QString type = QString(sgCmd.callMethod("String", "type", 0, 0).getStringCString()).toLower();

    QSetIterator<SHCmdDescription> ite(m_shCmdRegistration->getCmdDescriptionSet());
    while(ite.hasNext())
    {
      SHCmdDescription cmdDescription = ite.next();
      if(type == cmdDescription.cmdType.toLower()) 
      {
        int id = QMetaType::type(cmdDescription.cmdType.toUtf8().constData());
        if(id != 0) 
        {
          SHCmd *cmd = static_cast <SHCmd*>(QMetaType::construct(id));
          cmd->setScene(m_shGLScene);
          addCommand(cmd->getFromRTVal(sgCmd));
          delete cmd;
          break;
        }
      }
    }
  }

  onSynchronizeCommands();
}

void SHCmdHandler::onSynchronizeCommands() {
  m_stackSize = m_shGLScene->getNumCmdInUndoStack();
}

void SHCmdHandler::onSceneUpdated(SHGLScene *scene) { 
  m_shGLScene = scene; 
}
