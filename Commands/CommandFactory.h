//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_FACTORY__
#define __UI_COMMAND_FACTORY__

#include <QString> 
#include "BaseCommand.h"
#include "CommandRegistry.h"

namespace FabricUI {
namespace Commands {

class BaseCommandFactory
{
  /**
    BaseCommandFactory is the base class of all the CommandFactory.

    Each CommandFactory is associated to one type of command only, 
    and, the factories are registered in the CommandRegistry. 

    The userData argument is void pointer used to pass optional custom data to the command.
    The data is referenced by the factory, and its passed to the command throught
    the BaseCommand::registrationCallBack callback.
  */

  public:
  	BaseCommandFactory(
      void *userData
      );

  	virtual ~BaseCommandFactory();

    /// Create the command, called from CommandRegistry.
    /// To override.
  	virtual BaseCommand *createCommand();

    /// The user data. Stored by the factory and
    /// passed to the command when it's created.
  	void *m_userData;
};
 
template<typename T> 
class CommandFactory : public BaseCommandFactory
{
  /**
    CommandFactory is used to register commands in the CommandRegistry.
    To register a command: CommandFactory<cmdType>::RegisterCommand(cmdName, userData);
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
	    CommandRegistry::GetCommandRegistry()->registerFactory(
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
