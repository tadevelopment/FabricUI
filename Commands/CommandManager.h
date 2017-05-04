//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_MANAGER__
#define __UI_COMMAND_MANAGER__

#include <QMap>
#include <QList>
#include <QObject>
#include <QString>
#include "Command.h"

// Need to use a typedef because gcc doesn't support templated default arguments:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=39426 
typedef QMap<QString, QString> QMapString;

namespace FabricUI {
namespace Commands {

class CommandManager : public QObject
{
  /**
    CommandManager contains and manages the command undo-redo stack. It's in charge of creating  
    the commands, executing and undo-redoing them. CommandManager is specialized depending on   
    the command registration system, C++ vs Phyton,. See CommandManager for C++ implementation, 
    CommandManager_Python and Python/Canvas/CommandManager.py for Python. 

    The manager is shared between C++ and Python, so commands defined in Python can be created
    from C++, and vice versa. The manager can only create commands /C++/Python) registered in 
    the registry. When a command is added to the stack, the signal `commandPushed` is emitted. 
    
    The manager sets it-self as a singleton when it's constructed:
    - Create the singleton: CommandManager *cmdManager = new CommandManager();
  
    - Get the singleton: CommandManager *cmdManager = CommandManager::GetCommandManager();

    Usage:
    - Create a command (C++/Python): Command *cmd = cmdManager->createCommand(name, args, do);
        
    - Execute a command: cmdManager->doCommand(cmd);

    - Proces undo: cmdManager->undoCommand();

    - Proces redo: cmdManager->redoCommand();
  */
  Q_OBJECT

  public:
    CommandManager();

    virtual ~CommandManager();
  
    /// Gets the manager singleton.
    /// Thows an error if the manager has not been created.
    static CommandManager* GetCommandManager();
   
    /// Creates and executes a command (if doCmd == true).
    /// If executed, the command is added to the manager stack.
    /// Throws an exception if an error occurs.
    virtual Command* createCommand(
      const QString &cmdName, 
      const QMap<QString, QString> &args = QMapString(), 
      bool doCmd = true
      );

    /// Executes a command and adds it to the undo stack.
    /// Throws an exception if an error occurs.
    virtual void doCommand(
      Command* cmd
      );

    /// Undoes the current command.
    /// Throws an exception if an error occurs.
    virtual void undoCommand();

    /// Redoes the next command.
    /// Throws an exception if an error occurs.
    virtual void redoCommand();

    /// Clears all the commands.
    virtual void clear();

    /// Returns the number of commands.
    int count();

    /// Gets the current index (next command to undo).
    /// If -1 is returns, there is no command to undo.
    int getStackIndex();

    /// Gets the stack content as a string.
    /// Used for debugging.
    virtual QString getContent();

  signals:
    /// Emitted when a top command has 
    /// been succefully pushed to the stack.
    void commandPushed(
      Command *cmd
      );

    /// Emitted when the manager is cleared.
    void cleared();

  protected:
    /// Checks the command arguments before doing it.
    /// Throws an exception if an error occurs.
    virtual void checkCommandArgs(
      Command *cmd,
      const QMap<QString, QString> &args
      );

    /// Inform a command has been pushed. 
    /// Needed by the python implementation.
    virtual void commandIsPushed(
      Command *cmd,
      bool isLowCmd = false
      );

    /// Clears the redo stack.
    virtual void clearRedoStack();

    /// Pushes a command.
    void pushTopCommand(
      Command *cmd,
      bool succeeded = false
      );

    /// Structure containing a command (top) and
    /// its sub-commands (low) in a flat array.
    struct StackedCommand 
    {
      bool succeeded;
      Command* topLevelCmd;
      QList<Command*> lowLevelCmds;

      StackedCommand() 
      {
        topLevelCmd = 0;
        succeeded = false;
      }
    };

    /// Undo-redo stacks
    QList<StackedCommand> m_undoStack, m_redoStack;

  private:
    /// Clears a specific stack.
    void clearCommandStack(
      QList<StackedCommand> &stackedCmd
      );

    /// Pushes a sub-command.
    void pushLowCommand(
      Command *cmd
      );
 
    /// Gets a specific stack content 
    /// as a string, used for debugging.
    QString getStackContent(
      const QString& stackName, 
      const QList<StackedCommand> &stack
      );

    /// Cleans the command stack if errors 
    /// occur when doing a command.
    void cleanupUnfinishedCommands();

    /// CommandManager singleton, set from Constructor.
    static CommandManager *s_cmdManager;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
};
 
} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_MANAGER__
