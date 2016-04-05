/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_H__
#define __UI_SCENEHUB_CMD_H__

#include <QtGui/QUndoCommand>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <FabricUI/Util/StringUtils.h>
#include <FabricUI/SceneHub/SHGLScene.h>

namespace FabricUI
{
  namespace SceneHub
  {

    class SHCmd
    {
      public:
        /// Extracts the name of a command.
        /// \param command The command
        /// \param name The command's name
        static bool ExtractName(QString command, QString &name);

        /// Extracts the parameters from the command string.
        /// \param command The command
        /// \param params The array of parameters as string
        static bool ExtractParams(QString command, QStringList &params);

        /// Constructs and executes a command.
        /// \param shGLScene A reference to shGLScene
        /// \param name The name of the command
        /// \param desc The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SHCmd(SHGLScene *shGLScene, QString name, QString desc, QList<FabricCore::RTVal> &params, bool exec);

        ~SHCmd() {};

        /// Does nothing (don't call the command in KL).
        void doit();
        
        /// Undoes the command.
        void undo();
        
        /// Redoes the command.
        void redo();

        /// Gets the command description
        QString getDesc() const { assert(wasInvoked()); return m_desc; };
        
        /// Sets the command decription (here the command itself).
        /// \param desc The description
        void setDesc(QString desc) { m_desc = desc; };
 
        /// Adds additional dependencies of RTVals to this cmd
        void addRTValDependency(FabricCore::RTVal val);


      protected:
        /// Checks if the command has been already applied.
        bool wasInvoked() const { return m_state != State_New; };

        /// Command states
        enum State {
          State_New,
          State_Done,
          State_Undone,
          State_Redone
        };

        State m_state;
        QString m_desc;
        unsigned m_coreUndoCount;
        QList<FabricCore::RTVal> m_additionalRTVals;

        /// \internal
        SHGLScene *m_shGLScene;
    };

  }
}

#endif // __UI_SCENEHUB_CMD_H__
