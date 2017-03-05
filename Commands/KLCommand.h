//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND__
#define __UI_KL_COMMAND__

#include "BaseCommand.h"
#include <FabricCore.h>

namespace FabricUI {
namespace Commands {

class KLCommand : public BaseCommand
{
  /**
    KLCommand wraps the KL commands so they are accessible from C++/Python.
    They are created on-demand by the C++/Python registry.
    
    Usage:
    - KL:
      object MyCommand : Command {
        String name;
      };

      String MyCommand.getName() {
        return this.name;
      }

      MyCommand.setName!( String name ) {
        this.name = name;
      }

      Boolean MyCommand.canUndo() {
        return true;
      }

      Boolean MyCommand.doIt!( io String error ) {
        ... Do you logic
        return true;
      }

      Boolean MyCommand.undoIt!() {
        ... Undo it
        return true;
      }

      Boolean MyCommand.redoIt!() {
        ... Redo it
        return true;
      }
      
      // Register the command
      GetCommandRegistry().registerCommand(
        "myCommand", 
        MyCommand);

    - C++:
      // Create an execute the command
      CommandManager *manager = CommandManager.GetCommandManager();
      BaseCommand *cmd = manager->createCommand("myCommand") 

    - Python:
      // Create an execute the command
      manager = GetCommandManager()
      cmd = manager.createCommand("myCommand") 
  */

  public:
    KLCommand(
      FabricCore::RTVal klCmd
      );

    virtual ~KLCommand();

    /// Shared with KLSCriptableCommand.
    static QString GetName(
      FabricCore::RTVal klCmd
      );

    /// Shared with KLSCriptableCommand.
    static bool CanUndo(
      FabricCore::RTVal klCmd
      );

    /// Shared with KLSCriptableCommand.
    static bool DoIt(
      FabricCore::RTVal klCmd
      );

    /// Shared with KLSCriptableCommand.
    static QString GetHelp(
      FabricCore::RTVal klCmd
      );

    /// Shared with KLSCriptableCommand.
    static bool UndoIt();

    /// Shared with KLSCriptableCommand.
    static bool RedoIt();

    /// Implementation of BaseCommand.
    virtual QString getName();

    /// Implementation of BaseCommand.
    virtual bool canUndo();

    /// Implementation of BaseCommand.
    virtual bool doIt();

    /// Implementation of BaseCommand.
    virtual bool undoIt();

    /// Implementation of BaseCommand.
    virtual bool redoIt();

    /// Implementation of BaseCommand.
    virtual QString getHelp();

  private:
    /// KL Command
    FabricCore::RTVal m_klCmd;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND__
