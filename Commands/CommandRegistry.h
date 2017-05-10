//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMMAND_REGISTRY__
#define __UI_COMMAND_REGISTRY__

#include "Command.h"
#include <FabricUI/Util/Factory.h> 

namespace FabricUI {
namespace Commands {

class CommandRegistry : public Util::BaseFactoryRegistry
{
  /**
    CommandRegistry registers C++ command-factories and creates commands from them. 
    When a command is registered, the signal `commandRegistered` is emitted. When 
    specialized in Python , the same registry is shared between C++ and Python,
    so commands implemented in Python can be created from C++ and vice versa.
    
    The registry sets it-self as a singleton when it's constructed:
    - Create the singleton: new CommandRegistry();
    - Get the singleton: CommandRegistry *cmdRegistry = CommandRegistry::GetCommandRegistry();
  */  
  Q_OBJECT

  public:
    // Command type
    QString COMMAND_CPP; 
 
    CommandRegistry();

    virtual ~CommandRegistry();

    /// Gets the registry singleton.
    /// Thows an error if the registry has not been created.
    static CommandRegistry* GetCommandRegistry();
    
    /// Checks if the registry has been created.
    static bool IsInitalized();

    /// Creates a command named 'cmdName'.
    /// Throws an error if the command isn't registered.
    virtual Command* createCommand(
      const QString &cmdName
      );

    /// Checks if a command (C++/Python) 
    /// has been registered under 'cmdName'.
    bool isCommandRegistered(
      const QString &cmdName
      );

    /// Gets the command and implementation type (C++/Python).
    /// Returns an empty list if the command is not registred.
    QList<QString> getCommandSpecs(
      const QString &cmdName
      );

    /// Gets all the registred commands (C++/Python) 
    /// and the specs as a string, used for debugging.
    QString getContent();
  
  signals:
    /// Emitted when a command has been registered.
    /// \param cmdName The name of the command
    /// \param cmdType Object type
    /// \param implType Implementation : C++ or Python
    void commandRegistered(
      const QString &cmdName,
      const QString &cmdType,
      const QString &implType
      );

  public slots:
    /// Implementation of Util::FactoryRegistry.
    virtual void registerFactory(
      const QString &name, 
      Util::Factory *factory
      );

    /// Implementation of Util::FactoryRegistry.
    /// Does nothing.
    virtual void unregisterFactory(
      const QString &name 
      );
    
  protected:
    /// Informs a command has been registered. 
    /// \param cmdName The name of the command
    /// \param cmdType Object type
    /// \param implType Implementation : C++ or Python
    virtual void commandIsRegistered(
      const QString &cmdName,
      const QString &cmdType,
      const QString &implType
      );

  private:
    /// Dictionaries of registered commands (Python/C++): 
    /// {cmdName, {type, implementation type}}
    QMap< QString, QList<QString> > m_cmdSpecs;
    /// CommandRegistry singleton, set from Constructor.
    static CommandRegistry *s_cmdRegistry;
    /// Check if the singleton has been set.
    static bool s_instanceFlag;
};

template<typename T> 
class CommandFactory : public Util::TemplateFactory<T>
{
  /**
    CommandFactory is used to register commands in the CommandRegistry.
    - Register a command: CommandFactory<cmdType>::Register(cmdName, userData);
  */
  public:
    CommandFactory(void *userData) 
      : Util::TemplateFactory<T>(userData) 
    {
    }

    /// Registers the command <T> under "cmdName".
    static void Register(
      const QString &cmdName,
      void *userData=0) 
    {
      Util::TemplateFactory<T>::Register(
        CommandRegistry::GetCommandRegistry(),
        cmdName,
        userData);
    }
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_REGISTRY__
