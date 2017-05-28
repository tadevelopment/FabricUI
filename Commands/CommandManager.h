//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_MANAGER__
#define __UI_COMMAND_MANAGER__

#include <QMap>
#include <QList>
#include "BaseCommand.h"
#include <QSharedPointer>

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
    the command registration system, C++ vs Phyton, cf, CommandManager for C++ implementation, 
    CommandManager_Python and Python/Canvas/CommandManager.py for Python. 

    The manager is shared between C++ and Python, so commands defined in Python can be created
    from C++, and vice versa. The manager can only create commands /C++/Python) registered in 
    the registry. When a command is added to the stack, the signal `commandDone` is emitted. 
    
    The manager sets it-self as a singleton when it's constructed:
    - Create the singleton: new CommandManager();
    - Get the singleton: CommandManager *cmdManager = CommandManager::GetCommandManager();
  */
  Q_OBJECT

  public:
    CommandManager();

    virtual ~CommandManager();
  
    /// Gets the manager singleton.
    /// Thows an error if the manager has not been created.
    static CommandManager* GetCommandManager();
    
    /// Checks if the manager has been created.
    static bool IsInitalized();

    /// Creates and executes a command (if doCmd == true).
    /// If executed, the command is added to the manager stack.
    /// Throws an exception if an error occurs.
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

    /// Clears all the commands.
    virtual void clear();

    /// Returns the number of commands.
    unsigned count();

    /// Gets the current index (next command to undo).
    /// If -1 is returned, there is no command to undo.
    int getStackIndex();

    /// Gets the stack content as a string.
    /// Used for debugging.
    virtual QString getContent();

    /// Gets the command at index 'index'.
    BaseCommand* getCommandAtIndex(
      unsigned index
      );

  signals:
    /// Emitted when a top command has 
    /// been succefully pushed to the stack.
    void commandDone(
      BaseCommand *cmd
      );

    /// Emitted when the manager is cleared.
    void cleared();

  protected:
    /// Checks the command arguments before doing it.
    /// Throws an exception if an error occurs.
    virtual void checkCommandArgs(
      BaseCommand *cmd,
      const QMap<QString, QString> &args
      );

    /// Inform a command has been pushed. 
    /// Needed by the python implementation.
    virtual void commandPushed(
      BaseCommand *cmd,
      bool isLowCmd = false
      );

    /// Clears the redo stack.
    virtual void clearRedoStack();

    /// Pre-processes the args.
    /// Called before creating a command.
    virtual void preProcessCommandArgs(
      BaseCommand* cmd
      );

    /// Post-processes the args. Called after 
    /// creating, undoing or redoing a command.
    virtual void postProcessCommandArgs(
      BaseCommand* cmd
      );

    /// Pushes a command.
    virtual void pushTopCommand(
      BaseCommand *cmd,
      bool succeeded = false
      );

    /// Cleans the stacks if errors occur when
    /// doing a command and throws an exception.
    void cleanupUnfinishedCommandsAndThrow(
      BaseCommand *cmd,
      const QString& error = QString()
      );

    /// Contains a command (top) and its 
    /// sub-commands (low) in a flat array.
    struct StackedCommand 
    {
      bool succeeded;
      // Use shared pointer so th
      QSharedPointer< BaseCommand > topLevelCmd;
      QList< QSharedPointer<BaseCommand> > lowLevelCmds;

      StackedCommand() 
      {
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
      BaseCommand *cmd
      );
 
    /// Gets a specific stack content 
    /// as a string, used for debugging.
    QString getStackContent(
      const QString& stackName, 
      const QList<StackedCommand> &stack
      );

    /// Cleans the stacks if errors occur when
    /// undoing a command and throws an exception.
    void cleanupUnfinishedUndoLowCommandsAndThrow(
      int topLevelCmdIndex, 
      StackedCommand &stackedCmd,
      const QString &error = QString()
      );

    /// Cleans the stacks if errors occur when
    /// redoing a command and throws an exception.
    void cleanupUnfinishedRedoLowCommandsAndThrow(
      int topLevelCmdIndex, 
      StackedCommand &stackedCmd,
      const QString &error = QString()
      );

    /// CommandManager singleton, set from Constructor.
    static CommandManager *s_cmdManager;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
};
 
} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_MANAGER__
