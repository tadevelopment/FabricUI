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
    class SHCmdHandler : public QObject
    {
      Q_OBJECT

      protected:
        class WrappedCmd;

      public:
        /// Constructs a new SHCmdHandler.
        /// \param qUndoStack A pointer to the Qt undo-redo stack
        SHCmdHandler(SHGLScene *scene, QUndoStack *qUndoStack);
  
        virtual ~SHCmdHandler() {};

        bool addCommand(QString command, bool exec);

       
      public slots:
        void onSceneUpdated(SHGLScene *scene) { m_shGLScene = scene; }

        void onSynchronizeCommands();

        void onAddCommands();


      protected:        
        /// Adds and executes a command
        /// \param cmd The command
        bool addCommand(SHCmd *cmd);

        unsigned int m_stackSize;
        QUndoStack *m_qUndoStack;
        SceneHub::SHGLScene *m_shGLScene;
    };
  }
}

#endif // __UI_SCENEHUB_CMDHANDLER_QUNDO_H__
