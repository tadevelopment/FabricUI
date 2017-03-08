//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_COMMAND_MANAGER__
#define __UI_BASE_COMMAND_MANAGER__

#include <QMap>
#include <QList>
#include <QObject>
#include <QString>
#include <FabricCore.h>
#include "BaseCommand.h"
#include "CommandRegistry.h"

// Need to use a typedef because gcc doesn't support templated default arguments:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=39426 
typedef QMap<QString, QString> QMapString;

namespace FabricUI {
namespace Commands {

class CommandManager : public QObject
{
  /**
    CommandManager contains and manages the command undo-redo stack. It's in charge 
    of creating the commands, executing and undo-redoing them. It has a reference  
    to the KL command manager so it can create KL commands. However, the C++ manager
    is not automatically synchronized with the KL manager. If commands are added to
    the KL manager directly, the C++ manager needs explicitly to synchronize with it. 
    Two scenarios are possible. First, the KL manager creates KL commands. Specials
    C++ commands (KLCommand-KLScriptableCommand) that wrap KL commands are created.
    Secondly, C++ commands are asked to be created frrom KL. An 'AppCommand' command
    is created in KL. When the two managers are synchronized, the C++ manager detects 
    the 'AppCommand' KL command, and creates the corresponding C++.

    CommandManager is specialized depending on the command registration system, C++
    vs Phyton, see CommandManager for the C++ implementation, CommandManager_Python
    and Python/Canvas/CommandManager.py for the Python one. The manager is shared 
    between C++ and Python, so commands defined in Python can be created from C++, 
    and vice versa. The manager can only create commands (KL/C++/Python) registered 
    in the command registry. When a command is added to the stack, the signal 
    `commandPushedCallback` is emitted. 
    
    The manager sets it-self as a singleton when it's constructed:
    - Create the manager: CommandManager cmdManager(fabricClient);
  
    - Get the manager: CommandManager *cmdManager = CommandManager::GetCommandManager();

    Usage:
    - Create a command (C++/KL/Python): BaseCommand *cmd = cmdManager->createCommand(cmdName, args, doIt);
    
    - Execute a command (C++/KL/Python): cmdManager->doCommand(cmd);
    
    - Proces undo: cmdManager->undoCommand();

    - Proces redo: cmdManager->redoCommand();

    - Synchronize with the KL manager: cmdManager->synchronizeKL();
  */
  Q_OBJECT

  public:
    CommandManager(
      FabricCore::Client client
      );

    virtual ~CommandManager();
  
    /// Gets the manager singleton.
    /// Thows an error if the manager has not been created.
    static CommandManager* GetCommandManager();
   
    /// Creates and executes a command (if doCmd == true).
    /// If executed, the command is added to the manager stack.
    /// Throws an exception if an error occurs.
    /// To override depending of the registration mechanism.
    virtual BaseCommand* createCommand(
      const QString &cmdName, 
      const QMap<QString, QString> &args = QMapString(), 
      bool doCmd = true
      );

    /// Executes a command and adds it to the undo stack.
    /// Throws an exception if an error occurs.
    void doCommand(
      BaseCommand* cmd
      );

    /// Undoes the current command.
    /// Throws an exception if an error occurs.
    void undoCommand();

    /// Redoes the next command.
    /// Throws an exception if an error occurs.
    void redoCommand();

    /// Clears all the commands.
    virtual void clear();

    /// Returns the number of commands.
    int count();

    /// Gets the current index (next command to undo).
    /// If -1 is returns, there is no command to undo.
    int getStackIndex();

    /// Gets the KL comand manager.
    FabricCore::RTVal getKLCommandManager();

    /// Gets the FabricCore client.
    FabricCore::Client getFabricClient();
    
    /// Synchronizes with the KL manager.
    /// To call explicitly after KL commands 
    /// have been created from KL.
    void synchronizeKL();

    /// Gets the stack content as a string.
    /// Used for debugging.
    QString getContent();

  signals:
    /// Emitted when a command has 
    /// been pushed to the stack.
    void commandPushedCallback(
      BaseCommand *cmd,
      bool isLowCmd
      );

  protected slots:
    /// Called when a command has been registered,
    /// needed by the python implementation.
    virtual void onCommandRegistered(
      const QString &cmdName,
      const QString &cmdType,
      const QString &implType
    );

  protected:
    /// Checks the command arguments before doing it.
    /// Throws an exception if an error occurs.
    void checkCommandArgs(
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

    /// Structure containing a command (top) and
    /// its sub-commands (low) in a flat array.
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

    /// Undo-reso stacks
    QList<StackedCommand> m_undoStack, m_redoStack;

  private:
    /// Clears a specific stack.
    void clearCommandStack(
      QList<StackedCommand> &stackedCmd
      );

    /// Pushes a command.
    void pushTopCommand(
      BaseCommand *cmd,
      bool succeeded = false
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

    /// Cleans the command stack if errors 
    /// occur when doing a command.
    void cleanupUnfinishedCommands();

    /// KL command manager.
    FabricCore::RTVal m_klCmdManager;
    /// Fabric client.
    FabricCore::Client m_client;
    /// Number of KLCommand (KLScriptableCommand)
    /// added, use to compare wit the KL manager.
    unsigned int m_klCmdUndoStackCount;
    /// CommandManager singleton, set from Constructor.
    static CommandManager *s_cmdManager;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
};
 
} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_COMMAND_MANAGER__
