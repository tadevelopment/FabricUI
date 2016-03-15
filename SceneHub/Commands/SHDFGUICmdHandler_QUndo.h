/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMDHANDLER_QUNDO_H__
#define __UI_SCENEHUB_CMDHANDLER_QUNDO_H__

#include "SHCmd.h"
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtGui/QUndoStack>
#include <QtCore/QStringList>
#include <FabricUI/Util/macros.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGUICmdHandler_QUndo.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHDFGUICmdHandler_QUndo : public DFG::DFGUICmdHandler_QUndo
    {
      protected:
        class WrappedCmd;

      public:
        /// Default constructor
        SHDFGUICmdHandler_QUndo() : DFG::DFGUICmdHandler_QUndo() {};

        /// Constructs a new SHDFGUICmdHandler_QUndo.
        /// \param qUndoStack A pointer to the Qt undo-redo stack
        SHDFGUICmdHandler_QUndo(QUndoStack *qUndoStack) : DFG::DFGUICmdHandler_QUndo(qUndoStack) {};
  
        virtual ~SHDFGUICmdHandler_QUndo() {};

        // ***** Canvas *****
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoRemoveNodes(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QStringList nodeNames
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoConnect(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString srcPath, 
          QString dstPath
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoDisconnect(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString srcPath, 
          QString dstPath
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoAddGraph(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString title,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoAddFunc(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString title,
          QString initialCode,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoInstPreset(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString presetPath,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoAddVar(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString desiredNodeName,
          QString dataType,
          QString extDep,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoAddGet(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString desiredNodeName,
          QString varPath,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoAddSet(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString desiredNodeName,
          QString varPath,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoAddPort(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString desiredPortName,
          FabricCore::DFGPortType portType,
          QString typeSpec,
          QString portToConnect,
          QString extDep,
          QString metaData
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoCreatePreset(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString nodeName,
          QString presetDirPath,
          QString presetName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoEditPort(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString oldPortName,
          QString desiredNewPortName,
          QString typeSpec,
          QString extDep,
          QString uiMetadata
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoRemovePort(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString portName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoResizeBackDrop(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString backDropNodeName,
          QPointF newTopLeftPos,
          QSizeF newSize
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo        
        virtual void dfgDoMoveNodes(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QStringList nodeNames,
          QList<QPointF> newTopLeftPoss
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoImplodeNodes(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QStringList nodeNames,
          QString desiredNodeName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QStringList dfgDoExplodeNode(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString nodeName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoAddBackDrop(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString title,
          QPointF pos
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetNodeComment(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString nodeName,
          QString comment
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetCode(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString code
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoEditNode(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString oldNodeName,
          QString desiredNewNodeName,
          QString nodeMetadata,
          QString execMetadata
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QString dfgDoRenamePort(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString oldName,
          QString desiredNewName
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual QStringList dfgDoPaste(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString json,
          QPointF cursorPos
          );

        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetArgValue(
          FabricCore::DFGBinding const &binding,
          QString argName,
          FabricCore::RTVal const &value
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetPortDefaultValue(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString portOrPinPath,
          FabricCore::RTVal const &value
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetRefVarPath(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QString refName,
          QString varPath
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoReorderPorts(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QList<int> indices
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSetExtDeps(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec,
          QStringList extDeps
          );
        /// Specialization of DFG::DFGUICmdHandler_QUndo
        virtual void dfgDoSplitFromPreset(
          FabricCore::DFGBinding const &binding,
          QString execPath,
          FabricCore::DFGExec const &exec
          );

        /// Gets a pointer to the qt command stack.
        QUndoStack* getStack() { return m_qUndoStack; };
        
        /// Adds and executes a command
        /// \param cmd The command
        bool addCommand(SHCmd *cmd);
    };
  }
}

#endif // __UI_SCENEHUB_CMDHANDLER_QUNDO_H__

