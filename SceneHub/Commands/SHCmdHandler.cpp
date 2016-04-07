/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SGAddObjectCmd.h"
#include "SGAddPropertyCmd.h"
#include "SGSetPropertyCmd.h"
#include "SGSetPaintToolAttributeCmd.h"
#include "SHCmdHandler.h"
   
using namespace FabricCore;
using namespace FabricUI;
using namespace FabricUI::SceneHub;


class SHCmdHandler::WrappedCmd : public QUndoCommand {

  public:
    WrappedCmd( SHCmd *shCmd ) : QUndoCommand(), m_shCmd( shCmd ), m_didit( false ) {};

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
        QUndoCommand::setText( m_shCmd->getDesc() );
      }
      catch (FabricCore::Exception e) {
        printf("Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

    virtual void undo() {
      try 
      {
        assert( m_didit );
        m_shCmd->undo();
      }
      catch (FabricCore::Exception e) {
        printf( "Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

  private:
    FTL::OwnedPtr<SHCmd> m_shCmd;
    bool m_didit;
};

SHCmdHandler::SHCmdHandler(SHGLScene *scene, QUndoStack *qUndoStack)
 : m_qUndoStack(qUndoStack)
 , m_shGLScene(scene)
 , m_stackSize(0) {
}

bool SHCmdHandler::addCommand(SHCmd *cmd) {
  if(cmd) 
  {
    m_qUndoStack->push( new WrappedCmd(cmd) );
    return true;
  }
  else return false;
}

bool SHCmdHandler::addCommand(QString command, bool exec) {
  QString name;
  if(SHCmd::ExtractName(command, name)) 
  {
    // SGAddObjectCmd
    if(name.toLower() == SGAddObjectCmd_Str.toLower()) 
      return addCommand(SGAddObjectCmd::Create(m_shGLScene, command, exec));
    
    // SGAddPropertyCmd
    else if(name.toLower() == SGAddPropertyCmd_Str.toLower()) 
      return addCommand(SGAddPropertyCmd::Create(m_shGLScene, command, exec));
    
    // SGSetPropertyCmd
    else if(name.toLower() == SGSetPropertyCmd_Str.toLower()) 
      return addCommand(SGSetPropertyCmd::Create(m_shGLScene, command, exec));

    // SGSetPaintToolAttributeCmd
    else if(name.toLower() == SGSetPaintToolAttributeCmd_Str.toLower())
      return addCommand(SGSetPaintToolAttributeCmd::Create(m_shGLScene, command, exec));
 
    // log an error
    else std::cerr << "SHCmdHandler::addCommand : Command name " << name.toStdString() << " wasn't founded" << std::endl;
  }
  return false;
}

void SHCmdHandler::onSynchronizeCommands() {
  m_stackSize = m_shGLScene->getNumCmdInUndoStack();
}

void SHCmdHandler::onAddCommands() {

  // Get the number of commands already done in the KL stack
  for(unsigned int i=m_stackSize; i<m_shGLScene->getNumCmdInUndoStack(); ++i)
  {
    FabricCore::RTVal typeVal = m_shGLScene->retrieveCmd(i).callMethod("String", "type", 0, 0);
    QString type = QString(typeVal.getStringCString());
      
    if(type.toLower() == SGAddObjectCmd_Type_Str.toLower())
      addCommand(SGAddObjectCmd::Get(m_shGLScene, i), false);

    else if(type.toLower() == SGAddPropertyCmd_Type_Str.toLower())
      addCommand(SGAddPropertyCmd::Get(m_shGLScene, i), false);

    else if(type.toLower() == SGSetPropertyCmd_Type_Str.toLower())
      addCommand(SGSetPropertyCmd::Get(m_shGLScene, i), false);

    else if(type.toLower() == SGSetPaintToolAttributeCmd_Type_Str.toLower())
      addCommand(SGSetPaintToolAttributeCmd::Get(m_shGLScene, i), false);

    // log an error
    else std::cerr << "SHCmdHandler::synchronize : Command type " << type.toStdString() << " wasn't founded" << std::endl;
  }

  m_stackSize = m_shGLScene->getNumCmdInUndoStack();
}
