//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_COMMAND__
#define __UI_BASE_COMMAND__

#include <QObject>
#include <QString>

namespace FabricUI {
namespace Commands {

class BaseCommand : public QObject
{
  /**
    BaseCommand defines the functionalities of a command.
      
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

        virtual bool canUndo() 
        {
          return true;
        }

        virtual bool doIt() 
        {
          ... Do you logic

          --> Create a sub command
          CommandManager *manager = CommandManager.GetCommandManager();
          BaseCommand *mySubCommand = manager->createCommand("mySubCommand")
          
          return true;
        }

        virtual bool undoIt() 
        {
          ... Undo it
          return true;
        }

        virtual bool redoIt() 
        {
          ... Redo it
          return true;
        }
      };
      
      // Register the command
      CommandFactory<MyCommand>::Register("myCommand");

      // Create an execute the command
      CommandManager *manager = CommandManager.GetCommandManager();
      BaseCommand *cmd = manager->createCommand("myCommand") 

    - Python:
      class MyCommand(Commands.BaseCommand):
          
        def __ini__(self):
          super(MyCommand, self)__init__()  

        def registrationCallback(self, name, userData:
          super(MyCommand, self)registrationCallback(name, userData)  
          myUserData = userData
      
        def canUndo(self):
          return True
        
        def doIt(self):
          ... Do you logic

          --> Create a sub command
          mySubCommand = GetCommandManager().createCommand("mySubCommand")
          return True

        def undoIt(self):
          ... Undo it
          return True
        
        def redoIt(self):
          ... Redo it
          return True
      
      // Register the command
      GetCommandRegistry().registerCommand("myCommand", MyCommand)

      // Create an execute the command
      myCommand = GetCommandManager().createCommand("myCommand")

    C++ interfaces cannot be wrapped in python by shiboken. New commands
    must specialize this class to be accessible from python.
  */

  Q_OBJECT
  
  public:
    BaseCommand();

    virtual ~BaseCommand();

    /// Called when the command is created.
    /// The userData argument is used to pass optional custom data.
    /// The data is referenced by the registry, and given to the
    /// command with this callback.
    virtual void registrationCallback(
      const QString &name, 
      void *userData
      );

    /// Gets the command name.
    virtual QString getName();

    /// Checks if the command is undoable.
    virtual bool canUndo();

    /// Checks if the command can be logged.
    virtual bool canLog();

    /// Defines the command logic.
    /// Returns true if succeded, false otherwise.
    virtual bool doIt();

    /// Defines the undo logic.
    /// Returns true if succeded, false otherwise.
    virtual bool undoIt();

    /// Defines the redo logic.
    /// Returns true if succeded, false otherwise.
    virtual bool redoIt();
    
    /// Gets the command's help (description)
    virtual QString getHelp();

    /// Gets a description of the command
    /// to display in the history stack (if one).
    virtual QString getHistoryDesc();

    /// Sets the interaction ID.
    virtual void setCanMergeID(
      int canMergeID
      );

    /// Gets the interaction ID.
    virtual int getCanMergeID();

    /// Checks if the command `cmd` can
    /// be merged with `this` command.
    virtual bool canMerge(
      BaseCommand *cmd
      );

    /// Merges a command `cmd` with `this` command.
    virtual void merge(
      BaseCommand *cmd
      );

  private:
    /// Name of the command.
    QString m_name;
    /// Interaction ID, for merging.
    int m_canMergeID;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_COMMAND__
