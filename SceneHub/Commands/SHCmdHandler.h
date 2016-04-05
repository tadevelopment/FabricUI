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


namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmdHandler 
    {
      protected:
        class WrappedCmd;


      public:
        /// Constructs a new SHCmdHandler.
        /// \param qUndoStack A pointer to the Qt undo-redo stack
        SHCmdHandler(FabricUI::SceneHub::SHGLScene *scene, QUndoStack *qUndoStack);
  
        virtual ~SHCmdHandler() {};

        /// Gets a pointer to the qt command stack.
        QUndoStack* getStack() { return m_qUndoStack; };
        
        /// Adds and executes a command
        /// \param cmd The command
        bool addCommand(SHCmd *cmd);

        bool addCommand(QString command, bool exec);

        void setScene(FabricUI::SceneHub::SHGLScene *scene) { m_shGLScene = scene; }


      public slots:
        void synchronize();


      protected:
        QUndoStack *m_qUndoStack;
        SceneHub::SHGLScene *m_shGLScene;
    };
  }
}

#endif // __UI_SCENEHUB_CMDHANDLER_QUNDO_H__
