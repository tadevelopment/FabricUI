//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_REGISTRY__
#define __UI_COMMAND_REGISTRY__

#include <QMap> 
#include <QObject>
#include <QString> 
#include <FabricCore.h>
#include "BaseCommand.h"

namespace FabricUI {
namespace Commands {

class BaseCommandFactory;

class CommandRegistry : public QObject
{
  /**
    CommandRegistry registers the C++ command-factories and creates commands from them. 
    It has a reference to tke KL command registry, and automatially creates the C++ 
    wrappers (see KLCommand and KLScriptableCommand) of any KL commandregistered in the
    KL command registry. When a command is registered, the signal `commandRegisteredCallback`
    is emitted.
    
    When specialized in Python, the same registry is shared between C++ and Python, 
    so commands implemented in Python can be created from C++ and vice versa.
    
    The registry sets it-self as a singleton when it's constructed:
    - Create the registry: CommandRegistry cmdRegistry(fabricClient);
  
    - Get the registry: CommandRegistry *cmdRegistry = CommandRegistry::GetCommandRegistry();

    Usage:
    - Register a command (C++): CommandFactory<cmdType>::RegisterCommand(cmdName, userData);
    
    - Check a command is registered (Python/C++/KL): cmdRegistry->isCommandRegistered(cmdName);
    
    - Get a command specs [type, implType] (Python/C++/KL): cmdRegistry->getCommandSpecs(cmdName);

    - Create a command (C++/KL): BaseCommand *cmd = cmdRegistry->createCommand(cmdName);

    - Synchronize with the KL registry: cmdRegistry->synchronizeKL();
  */  
  Q_OBJECT

  public:
    /// Command implementation types.
    static QString COMMAND_KL; 
    static QString COMMAND_CPP; 
    static QString COMMAND_PYTHON; 
 
	  CommandRegistry(
      FabricCore::Client client
      );

    virtual ~CommandRegistry();

    /// Gets the registry singleton.
    /// Thows an error if the registry has not been created.
    static CommandRegistry* GetCommandRegistry();

    /// Registers a C++ CommandFactory.
  	void registerFactory(
      const QString &cmdName, 
      BaseCommandFactory *factory
      );

    /// Checks if a command (KL, C++, Python) 
    /// has been registered under "cmdName".
    bool isCommandRegistered(
      const QString &cmdName
      );

    /// Provides the command and implementation type (KL/C++/Python).
    /// Returns an empty list if the command is not registred.
    QList<QString> getCommandSpecs(
      const QString &cmdName
      );

    /// Creates a C++ or KL command named "cmdName".
    /// Throws an error if the command cannot be created,
    /// has to be registered first.
    virtual BaseCommand* createCommand(
      const QString &cmdName
      );

    /// Gets the KL registry.
    FabricCore::RTVal getKLRegistry();

    /// Gets the FabricCore client.
    FabricCore::Client getFabricClient();

    /// Synchronizes with the KL registry.
    /// Allows to create KL command from C++/Python.
    void synchronizeKL();

    /// Gets all the registred commands (KL/C++/Python) 
    /// and the specs as a string, used for debugging.
    QString getContent();
  
  signals:
    /// Emitted when a command 
    /// has been registered.
    void commandRegisteredCallback(
      const QString &cmdName,
      const QString &cmdType,
      const QString &implType
      );

  protected:
    /// Inform a command has been registered. 
    /// \param cmdName The name of the command
    /// \param cmdType Object type
    /// \param implType Implementation : KL, C++ or Python
    void commandRegistered(
      const QString &cmdName,
      const QString &cmdType,
      const QString &implType
    );

  private:
    /// Registers a command defined in KL.
    /// Allows to create KL command from C++/Python.
    void registerKLCommand(
      const QString &cmdName
      );

    /// Creates a C++ KLCommand or KLSriptableCommand wrapper 
    /// for a KL command registered in the KL CommandRegistry.
    BaseCommand* createKLCommand(
      const QString &cmdName
      );

    /// CommandRegistry singleton, set from Constructor.
    static CommandRegistry *s_cmdRegistry;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;

    /// Dictionaries of registered commands factories (C++): 
    QMap<QString, BaseCommandFactory*> m_registeredCmdFactories;
    /// Dictionaries of registered commands (Python/C++/KL): 
    /// {cmdName, {type, implementation type}}
    QMap< QString, QList<QString> > m_registeredCmdSpecs;
    /// KL command registry.
    FabricCore::RTVal m_klCmdRegistry;
    /// Fabric client.
    FabricCore::Client m_client;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_REGISTRY__
