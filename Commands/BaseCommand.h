//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_COMMAND__
#define __UI_BASE_COMMAND__

#include <QString>

namespace FabricUI {
namespace Commands {

class BaseCommand
{
  /**
    BaseCommand defines the functionalities of a command.
    Any command must inherit from this class. 

    The command manager supports implicit meta-commands. 
    Any command created within an other is automatically 
    its child and its pushed in a flat-stack owned by the
    command manager. When the top-level command is asked 
    to redo-redo, meta, the manager recursively redo-undo
    its sub-commands.
     
    Because the manager manages the sub-commands, a meta
    command cannot implements it's own undoable logic. 
     
    Usage:
    - C++:
      class MyCommand : public BaseCommand {
          
        MyCommand() : BaseCommand()
        {
        }

        virtual ~MyCommand()
        {
        }

        virtual void registrationCallback(
          const QString &name, 
          void *userData)
        {
          BaseCommand::registrationCallback(
            name, 
            userData);

          // Cast the pointer
          float *myUserData = (float*)userData;
        }

        virtual bool canUndo() {
          return true;
        }

        virtual bool doIt() {

          ... Do you logic
          --> Create a sub command
          CommandManager *manager = CommandManager.GetCommandManager();
          BaseCommand *mySubCommand = manager->createCommand("mySubCommand")
          
          return true;
        }

        virtual bool undoIt() {
          ... Undo it
          return true;
        }

        virtual bool redoIt() {
          ... Redo it
          return true;
      };
      
      // Register the command
      float userData = 32;
      CommandFactory<MyCommand>::RegisterCommand("myCommand", &userData);

      // Create an execute the command
      CommandManager *manager = CommandManager.GetCommandManager();
      BaseCommand *cmd = manager->createCommand("myCommand") 

    - Python:
      // Create an execute the command
      manager = GetCommandManager()
      cmd = manager.createCommand("myCommand") 
  */

  public:
    BaseCommand();

    virtual ~BaseCommand();

    /// Called when the command is created.
    /// The userData argument is used to pass optional custom data.
    /// The data is referenced by the registry, and given to the
    /// command with this callback.
    /// To override.
    virtual void registrationCallback(
      const QString &name, 
      void *userData
      );

    /// Gets the command name.
    virtual QString getName();

    /// Checks if the command is undoable (false by default).
    /// To override
    virtual bool canUndo();

    /// Defines the command logic.
    /// Returns true if succeded, false otherwise.
    /// To override.
    virtual bool doIt();

    /// Defines the undo logic.
    /// Returns true if succeded, false otherwise.
    /// To override.
    virtual bool undoIt();

    /// Defines the redo logic.
    /// Returns true if succeded, false otherwise.
    /// To override.
    virtual bool redoIt();

  protected:
    /// Name of the command.
    QString m_name;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_COMMAND__
