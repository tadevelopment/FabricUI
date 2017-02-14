//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_FACTORY__
#define __UI_COMMAND_FACTORY__

#include <QMap> 
#include <QString> 
#include "BaseCommand.h"

namespace FabricUI {
namespace Commands {

class BaseCommandFactory;

class CommandFactoryRegistry
{
  /**
    CommandFactoryRegistry registers all the command-factories and
    allows to create commands from them.

    To register a command: CommandFactory<cmdType>::RegisterCommand(cmdName, userData);
    To create a command: BaseCommand *cmd = CommandFactoryRegistry::CreateCommand(cmdName);
    To check that a command is registered: CommandFactoryRegistry::IsCommandRegistered(cmdName, factoryType);
  */

  public:
	  CommandFactoryRegistry();

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

  private:
    /// \internal
    /// Maps each command factory with a name.
    static QMap<QString, BaseCommandFactory*> s_registeredCmds;
};

class BaseCommandFactory
{
  /**
    BaseCommandFactory is the base class of all the CommandFactory.
    Each CommandFactory is associated to one type of command only, 
    and, the factories are registered in CommandFactoryRegistry. 
  */

  public:
  	BaseCommandFactory(
      void *userData
      );

  	virtual ~BaseCommandFactory();

    /// Create the command, called from CommandFactoryRegistry.
    /// To override.
  	virtual BaseCommand *createCommand();

    /// The user data. It is stored by the factory 
    /// so it can be given to the command when it's created.
  	void *m_userData;
};
 
template<typename T> 
class CommandFactory : public BaseCommandFactory
{
  /**
    CommandFactory is used to register commands in the CommandFactoryRegistry.
    To register a command: CommandFactory<cmdType>::RegisterCommand(cmdName, userData);

    The userData argument is void pointer used to pass optional custom data to the command.
    The data is referenced by the factory, and its passed to the command throught
    the BaseCommand::registrationCallBack callback.
  */

  public:
	  CommandFactory(void *userData) 
      : BaseCommandFactory(userData) 
    {}

	  virtual ~CommandFactory() {}

    /// Registers the command <T> under the name "cmdName".
    static void RegisterCommand(
      const QString &cmdName,
      void *userData = 0) 
    {
	    CommandFactoryRegistry::RegisterFactory(
        cmdName, 
        new CommandFactory(userData)
        );
    }

  	/// \internal
    /// Implementation of BaseCommandFactory.
    virtual BaseCommand *createCommand() 
    { 
  		return new T();
    } 
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_FACTORY__
