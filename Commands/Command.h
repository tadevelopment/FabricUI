//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND__
#define __UI_COMMAND__

#include <QString>

namespace FabricUI {
namespace Commands {

class Command
{
  /**
    Command defines the functionalities of a command.
      
    Usage:
    - C++:
      class MyCommand : public Command {
          
        MyCommand() : Command()
        {
        }

        virtual ~MyCommand()
        {
        }

        virtual void registrationCallback(
          const QString &name, 
          void *userData)
        {
          Command::registrationCallback(
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
          Command *mySubCommand = manager->createCommand("mySubCommand")
          
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
      Command *cmd = manager->createCommand("myCommand") 

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
  */
      
  public:
    /// Called when the command is created.
    /// The userData argument is used to pass optional custom data.
    /// The data is referenced by the registry, and given to the
    /// command with this callback.
    virtual void registrationCallback(
      const QString &name, 
      void *userData
      ) = 0;

    /// Gets the command name.
    virtual QString getName() = 0;

    /// Checks if the command is undoable, false by default.
    virtual bool canUndo() = 0;

    /// Defines the command logic.
    /// Returns true if succeded, false otherwise.
    virtual bool doIt() = 0;

    /// Defines the undo logic.
    /// Returns true if succeded, false otherwise.
    virtual bool undoIt() = 0;

    /// Defines the redo logic.
    /// Returns true if succeded, false otherwise.
    virtual bool redoIt() = 0;
    
    /// Gets the command's help (description)
    virtual QString getHelp() = 0;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND__
