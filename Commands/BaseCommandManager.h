//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_COMMAND_MANAGER__
#define __UI_BASE_COMMAND_MANAGER__

#include <QMap>
#include <QList>
#include <QString>
#include "BaseCommand.h"


// Need to use a typedef because gcc doesn't support templated default arguments:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=39426 
typedef QMap<QString, QString> QMapString;

namespace FabricUI {
namespace Commands {

class BaseCommandManager
{
  /**
    BaseCommandManager contains and manages the command undo-redo stack.
    It's in charge of creating the commands, executing and undo-redoing them.

    It is specialized depending on the command registration system (C++ vs Phyton).
    See CommandManager.h(cpp) for the C++ implementation, BaseCommandManager_Python.h(cpp)
    Python/Canvas/CommandManager.py for the Python one.
  
    The manager is shared between the C++ and Python, so commands defined in Python 
    can be created from C++ code too, and vice versa.

    The manager can only create commands (C++/Python) registered in CommandRegistry
    Commands registered in C++ (CommandFactory<cmdType>::RegisterCommand(cmdName)),
    are owned by the manager undo-redo stacks. Commands registered in Python are 
    owns by Python, and are only referenced by the stacks.
  */

  public:
    BaseCommandManager();

    virtual ~BaseCommandManager();
 
    /// Creates and executes a command (if doCmd == true).
    /// If executed, the command is added to manager stack.
    /// Throws an exception if an error occurs.
    /// To overwrite depending of the registration mechanism.
    virtual BaseCommand* createCommand(
      const QString &cmdName, 
      const QMap<QString, QString> &args = QMapString(), 
      bool doCmd = true
      );

    /// Executes a command and adds it to the undo stack.
    /// Throws an exception if an error occurs.
    virtual void doCommand(
      BaseCommand* cmd
      );

    /// Undoes the current command.
    /// Throws an exception if an error occurs.
    virtual void undoCommand();

    /// Redoes the next command.
    /// Throws an exception if an error occurs.
    virtual void redoCommand();

    /// Clears the commands.
    virtual void clear();

    /// Returns the number of commands.
    int count() const;

    /// Gets the current index (next command to undo).
    /// If -1 is returns, there is no command to undo anymore.
    int getStackIndex() const;
    
    /// Gets the stack content, used for debugging.
    virtual QString getContent() const;

  protected:
    /// \internal
    /// Gets the reference to the manager singleton.
    static BaseCommandManager* GetCommandManager();
   
    /// \internal
    /// Sets the reference to the manager singleton.
    /// Used in the Python implementation.
    static void SetCommandManagerSingleton(
      BaseCommandManager *instance
      );

    /// \internal
    struct StackedCommand 
    {
      bool succeeded;
      BaseCommand* topLevelCmd;
      QList<BaseCommand*> lowLevelCmds;

      StackedCommand() 
      {
        topLevelCmd = 0;
        succeeded = false;
      }
    };

    /// \internal
    /// Throws an exception if an error occurs.
    virtual void checkCommandArgs(
      BaseCommand *cmd,
      const QMap<QString, QString> &args
      ) const;
      
    /// \internal
    /// Called when a command (top or low) has been pushed 
    /// to the undo stack (used by the python wrapping).
    /// To override.
    virtual void commandPushedCallback(
      BaseCommand *cmd
      );

    /// \internal
    virtual void clearRedoStack();

    /// \internal
    void clearCommandStack(
      QList<StackedCommand> &stackedCmd
      );

    /// \internal
    virtual QString getStackContent(
      const QString& stackName, 
      const QList<StackedCommand> &stack
      ) const;

    /// \internal
    QList<StackedCommand> m_undoStack;
    QList<StackedCommand> m_redoStack;

  private:
    /// \internal
    static bool s_instanceFlag;
    static BaseCommandManager *s_cmdManager;
};
 
} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_COMMAND_MANAGER__
