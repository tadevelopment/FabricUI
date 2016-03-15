/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/DFG/DFGUICmd/DFGUICmds.h>
#include <FabricUI/SceneHub/Commands/SHDFGUICmdHandler_QUndo.h>

using namespace FabricCore;
using namespace FabricUI;
using namespace FabricUI::DFG;
using namespace FabricUI::SceneHub;


/// \internal
/// Wrap a SHCmd into a Qt command
class SHDFGUICmdHandler_QUndo::WrappedCmd : public QUndoCommand {

  public:
    WrappedCmd( SHCmd *shCmd ) : QUndoCommand(), 
      m_shCmd( shCmd ),
      m_didit( false ) 
      {
        m_dfgUICmd = 0;
      };

    WrappedCmd( FabricUI::DFG::DFGUICmd *dfgUICmd ) : QUndoCommand(), 
      m_dfgUICmd( dfgUICmd ),
      m_didit( false )
      {
        m_shCmd = 0;
      };


  protected:
    virtual void redo() 
    {
      try 
      {
        if(!m_dfgUICmd && m_shCmd)
        {
          if(m_didit) 
          {
            m_shCmd->redo();
          }
          else 
          {
            m_didit = true;
            m_shCmd->doit();
          }
          QUndoCommand::setText( m_shCmd->getDesc().c_str() );
        }
        else if(m_dfgUICmd && !m_shCmd) 
        {
          if ( m_didit )
          {
            m_dfgUICmd->redo();
          }
          else
          {
            m_didit = true;
            m_dfgUICmd->doit();
            QUndoCommand::setText( m_dfgUICmd->getDesc().toUtf8() );
          }
        }
      }
      catch (FabricCore::Exception e) {
        printf("Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

    virtual void undo() {
      try 
      {
        assert( m_didit );
        if(!m_dfgUICmd && m_shCmd)
        {
          m_shCmd->undo();
        }
        else if(m_dfgUICmd && !m_shCmd)
        {
          m_dfgUICmd->undo();
        }
      }
      catch (FabricCore::Exception e) {
        printf( "Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

  private:
    FTL::OwnedPtr<SHCmd> m_shCmd;
    FTL::OwnedPtr<FabricUI::DFG::DFGUICmd> m_dfgUICmd;
    bool m_didit;
};

// ***** DFG Commands *****
QString SHDFGUICmdHandler_QUndo::dfgDoInstPreset(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString presetPath,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_InstPreset(
      binding,
      execPath,
      exec,
      presetPath,
      pos
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

QString SHDFGUICmdHandler_QUndo::dfgDoAddVar(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString desireNodedName,
  QString dataType,
  QString extension,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddVar(
      binding,
      execPath,
      exec,
      desireNodedName,
      dataType,
      extension,
      pos
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

QString SHDFGUICmdHandler_QUndo::dfgDoAddGet(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString desireNodedName,
  QString varPath,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddGet(
      binding,
      execPath,
      exec,
      desireNodedName,
      varPath,
      pos
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

QString SHDFGUICmdHandler_QUndo::dfgDoAddSet(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString desireNodedName,
  QString varPath,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddSet(
      binding,
      execPath,
      exec,
      desireNodedName,
      varPath,
      pos
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

QString SHDFGUICmdHandler_QUndo::dfgDoAddGraph(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString title,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddGraph(
      binding,
      execPath,
      exec,
      title,
      pos
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

QString SHDFGUICmdHandler_QUndo::dfgDoAddFunc(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString title,
  QString initialCode,
  QPointF pos
  )
{
  DFGUICmd_AddNode *cmd =
    new DFGUICmd_AddFunc(
      binding,
      execPath,
      exec,
      title,
      initialCode,
      pos
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNodeName();
}

void SHDFGUICmdHandler_QUndo::dfgDoRemoveNodes(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QStringList nodeNames
  )
{
  m_qUndoStack->push(
    new WrappedCmd(
      new DFGUICmd_RemoveNodes(
        binding,
        execPath,
        exec,
        nodeNames
        )
      )
    );
}

void SHDFGUICmdHandler_QUndo::dfgDoConnect(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString srcPath, 
  QString dstPath
  )
{
  m_qUndoStack->push(
    new WrappedCmd(
      new DFGUICmd_Connect(
        binding,
        execPath,
        exec,
        srcPath,
        dstPath
        )
      )
    );
}

void SHDFGUICmdHandler_QUndo::dfgDoDisconnect(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString srcPath, 
  QString dstPath
  )
{
  m_qUndoStack->push(
    new WrappedCmd(
      new DFGUICmd_Disconnect(
        binding,
        execPath,
        exec,
        srcPath,
        dstPath
        )
      )
    );
}

QString SHDFGUICmdHandler_QUndo::dfgDoAddPort(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString desiredPortName,
  FabricCore::DFGPortType dfgPortType,
  QString typeSpec,
  QString portToConnect,
  QString extDep,
  QString metaData
  )
{
  DFGUICmd_AddPort *cmd =
    new DFGUICmd_AddPort(
      binding,
      execPath,
      exec,
      desiredPortName,
      dfgPortType,
      typeSpec,
      portToConnect,
      extDep,
      metaData
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualPortName();
}

QString SHDFGUICmdHandler_QUndo::dfgDoEditPort(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString oldPortName,
  QString desiredNewPortName,
  QString typeSpec,
  QString extDep,
  QString uiMetadata
  )
{
  DFGUICmd_EditPort *cmd =
    new DFGUICmd_EditPort(
      binding,
      execPath,
      exec,
      oldPortName,
      desiredNewPortName,
      typeSpec,
      extDep,
      uiMetadata
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNewPortName();
}

QString SHDFGUICmdHandler_QUndo::dfgDoCreatePreset(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString nodeName,
  QString presetDirPath,
  QString presetName
  )
{
  DFGUICmd_CreatePreset *cmd =
    new DFGUICmd_CreatePreset(
      binding,
      execPath,
      exec,
      nodeName,
      presetDirPath,
      presetName
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getPathname();
}

void SHDFGUICmdHandler_QUndo::dfgDoRemovePort(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString portName
  )
{
  DFGUICmd_RemovePort *cmd =
    new DFGUICmd_RemovePort(
      binding,
      execPath,
      exec,
      portName
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHDFGUICmdHandler_QUndo::dfgDoMoveNodes(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QStringList nodeNames,
  QList<QPointF> newTopLeftPoss
  )
{
  DFGUICmd_MoveNodes *cmd =
    new DFGUICmd_MoveNodes(
      binding,
      execPath,
      exec,
      nodeNames,
      newTopLeftPoss
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHDFGUICmdHandler_QUndo::dfgDoResizeBackDrop(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString backDropNodeName,
  QPointF newTopLeftPos,
  QSizeF newSize
  )
{
  DFGUICmd_ResizeBackDrop *cmd =
    new DFGUICmd_ResizeBackDrop(
      binding,
      execPath,
      exec,
      backDropNodeName,
      newTopLeftPos,
      newSize
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

QString SHDFGUICmdHandler_QUndo::dfgDoImplodeNodes(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QStringList nodeNames,
  QString desiredImplodedNodeName
  )
{
  DFGUICmd_ImplodeNodes *cmd =
    new DFGUICmd_ImplodeNodes(
      binding,
      execPath,
      exec,
      nodeNames,
      desiredImplodedNodeName
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualImplodedNodeName();
}

QStringList SHDFGUICmdHandler_QUndo::dfgDoExplodeNode(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString nodeName
  )
{
  DFGUICmd_ExplodeNode *cmd =
    new DFGUICmd_ExplodeNode(
      binding,
      execPath,
      exec,
      nodeName
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getExplodedNodeNames();
}

void SHDFGUICmdHandler_QUndo::dfgDoAddBackDrop(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString title,
  QPointF pos
  )
{
  DFGUICmd_AddBackDrop *cmd =
    new DFGUICmd_AddBackDrop(
      binding,
      execPath,
      exec,
      title,
      pos
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}
 
void SHDFGUICmdHandler_QUndo::dfgDoSetNodeComment(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString nodeName,
  QString comment
  )
{
  DFGUICmd_SetNodeComment *cmd =
    new DFGUICmd_SetNodeComment(
      binding,
      execPath,
      exec,
      nodeName,
      comment
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHDFGUICmdHandler_QUndo::dfgDoSetCode(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString code
  )
{
  DFGUICmd_SetCode *cmd =
    new DFGUICmd_SetCode(
      binding,
      execPath,
      exec,
      code
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

QString SHDFGUICmdHandler_QUndo::dfgDoRenamePort(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString oldName,
  QString desiredNewName
  )
{
  DFGUICmd_RenamePort *cmd =
    new DFGUICmd_RenamePort(
      binding,
      execPath,
      exec,
      oldName,
      desiredNewName
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNewPortName();
}

QString SHDFGUICmdHandler_QUndo::dfgDoEditNode(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString oldNodeName,
  QString desiredNewNodeName,
  QString nodeMetadata,
  QString execMetadata
  )
{
  DFGUICmd_EditNode *cmd =
    new DFGUICmd_EditNode(
      binding,
      execPath,
      exec,
      oldNodeName,
      desiredNewNodeName,
      nodeMetadata,
      execMetadata
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getActualNewNodeName();
}

QStringList SHDFGUICmdHandler_QUndo::dfgDoPaste(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString json,
  QPointF cursorPos
  )
{
  DFGUICmd_Paste *cmd =
    new DFGUICmd_Paste(
      binding,
      execPath,
      exec,
      json,
      cursorPos
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
  return cmd->getPastedNodeNames();
}

void SHDFGUICmdHandler_QUndo::dfgDoSetArgValue(
  FabricCore::DFGBinding const &binding,
  QString argName,
  FabricCore::RTVal const &value
  )
{
  DFGUICmd_SetArgValue *cmd =
    new DFGUICmd_SetArgValue(
      binding,
      argName,
      value
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHDFGUICmdHandler_QUndo::dfgDoSetPortDefaultValue(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString portOrPinPath,
  FabricCore::RTVal const &value
  )
{
  DFGUICmd_SetPortDefaultValue *cmd =
    new DFGUICmd_SetPortDefaultValue(
      binding,
      execPath,
      exec,
      portOrPinPath,
      value
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHDFGUICmdHandler_QUndo::dfgDoSetRefVarPath(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QString refName,
  QString varPath
  )
{
  DFGUICmd_SetRefVarPath *cmd =
    new DFGUICmd_SetRefVarPath(
      binding,
      execPath,
      exec,
      refName,
      varPath
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHDFGUICmdHandler_QUndo::dfgDoReorderPorts(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QList<int> indices
  )
{
  DFGUICmd_ReorderPorts *cmd =
    new DFGUICmd_ReorderPorts(
      binding,
      execPath,
      exec,
      indices
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHDFGUICmdHandler_QUndo::dfgDoSetExtDeps(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec,
  QStringList extDeps
  )
{
  DFGUICmd_SetExtDeps *cmd =
    new DFGUICmd_SetExtDeps(
      binding,
      execPath,
      exec,
      extDeps
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}

void SHDFGUICmdHandler_QUndo::dfgDoSplitFromPreset(
  FabricCore::DFGBinding const &binding,
  QString execPath,
  FabricCore::DFGExec const &exec
  )
{
  DFGUICmd_SplitFromPreset *cmd =
    new DFGUICmd_SplitFromPreset(
      binding,
      execPath,
      exec
      );
  m_qUndoStack->push( new WrappedCmd( cmd ) );
}


// ***** SceneHub Commands *****
/// Adds an object to the scene-graph
/// \param command The command as string
bool SHDFGUICmdHandler_QUndo::addCommand(SHCmd *cmd) {
  if(cmd) 
  {
    // Clears the kl undo stack --> synchronize Qt and K stacks
    //SHCmd::GetCmdManager(cmd->getRefOnSCeneHub()).callMethod("", "clearRedoStack", 0, 0);
    m_qUndoStack->push( new WrappedCmd(cmd) );
    return true;
  }
  else return false;
}


