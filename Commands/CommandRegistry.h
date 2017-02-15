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
    CommandRegistry registers all the command-factories and
    allows to create commands from them.

    To register a command: CommandFactory<cmdType>::RegisterCommand(cmdName, userData);
    To create a command: BaseCommand *cmd = CommandRegistry::CreateCommand(cmdName);
    To check that a command is registered: CommandRegistry::IsCommandRegistered(cmdName, factoryType);
  */

  public:
	  CommandRegistry();

    virtual ~CommandRegistry();

  	/// \internal
    /// Called from the factory
  	static void RegisterFactory(
      const QString &cmdName, 
      BaseCommandFactory *factory
      );

    /// Checks if a command has been registered under the name "cmdName".
    /// provide the name of the factory that creates the command: factoryType.
  	static bool IsCommandRegistered(
      const QString &cmdName,
      QString &factoryType
      );

    /// Creates a registered command named "cmdName".
    /// Throws an error if the command cannot be created (has to be registered first).
    static BaseCommand* CreateCommand(
      const QString &cmdName
      );

  protected:
    virtual void commandIsRegistered(const QString &cmdName);

  private:
    /// \internal
    /// Maps each command factory with a name.
    QMap<QString, BaseCommandFactory*> m_registeredCmds;
    static CommandRegistry s_cmdRegistry;
};


} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_REGISTRY__
