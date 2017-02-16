//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_MANAGER__
#define __UI_COMMAND_MANAGER__

#include "BaseCommandManager.h"
 
namespace FabricUI {
namespace Commands {

class CommandManager : public BaseCommandManager
{
  /**
    CommandManager specializes BaseCommandManager in C++.
    It is a singleton and should not be created directly.
    - Get the manager: BaseCommandManager *cmdManager = CommandManager::GetCommandManager()
  
    - Register a C++ command: CommandFactory<cmdType>::RegisterCommand(cmdName, userData);
    
    - Create a command (Python/C++): BaseCommand *cmd = cmdManager->createCommand(cmdName);
  */

  public:
    CommandManager();

    virtual ~CommandManager();

    /// Gets the reference to the manager singleton.
    /// Creates it if needed (if not created in Python)
    static BaseCommandManager* GetCommandManager();

    /// Implementation of BaseCommandManager
    BaseCommand* createCommand(
      const QString &cmdName, 
      const QMap<QString, QString> &args = QMapString(), 
      bool doCmd = true);
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_MANAGER__
