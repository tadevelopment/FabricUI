//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_REGISTRY__
#define __UI_COMMAND_REGISTRY__

#include <QMap> 
#include <QString> 
#include "BaseCommand.h"

namespace FabricUI {
namespace Commands {

class BaseCommandFactory;

class CommandRegistry
{
  /**
    CommandRegistry registers the command-factories and can create commands from them. 
    It is a singleton and should not be created directly.
    - Get the registry: CommandRegistry *cmdRegistry = CommandRegistry::GetCommandRegistry()
    
    The registery is shared between the C++ and Python, so commands defined in Python 
    can be called from C++ code too, and vice versa.

    - Register a C++ command: CommandFactory<cmdType>::RegisterCommand(cmdName, userData);
    
    - Check a command is registered (Python/C++): cmdRegistry.isCommandRegistered(cmdName, factoryType);
    
    - Create a command (Python/C++): BaseCommand *cmd = cmdRegistry.createCommand(cmdName);
  */

  public:
	  CommandRegistry();

    virtual ~CommandRegistry();

    /// Gets the reference to the registry singleton.
    /// Creates it if needed (if not created in Python)
    static CommandRegistry* GetCommandRegistry();

  	/// \internal
    /// Called from the factory.
  	void registerFactory(
      const QString &cmdName, 
      BaseCommandFactory *factory
      );

    /// Checks if a command has been registered under the name "cmdName".
    /// Provides the name of the factory that creates the command: factoryType.
    /// Can be overwridden
    bool isCommandRegistered(
      const QString &cmdName,
      QString &factoryType
      );

    /// Creates a registered command named "cmdName".
    /// Throws an error if the command cannot be created (has to be registered first).
    /// Can be overwridden
    BaseCommand* createCommand(
      const QString &cmdName
      );

  protected:
    /// \internal
    /// Sets the reference to the registery singeton.
    /// Used in the Python implementation.
    static void SetCommandRegistrySingleton(
      CommandRegistry *instance
      );

    /// \internal
    /// Called when a command is regitered.
    /// Used in the Python implementation.
    /// To overwride
    virtual void commandIsRegistered(
      const QString &cmdName,
      const QString &cmdType
    );

  private:
    /// \internal
    static bool s_instanceFlag;
    static CommandRegistry *s_cmdRegistry;
    /// Maps each command factory with a name.
    QMap<QString, BaseCommandFactory*> m_registeredCmds;
};


} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_REGISTRY__
