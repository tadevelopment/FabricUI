//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND_REGISTRY__
#define __UI_KL_COMMAND_REGISTRY__

#include <FabricCore.h>
#include "CommandRegistry.h"
 
namespace FabricUI {
namespace Commands {

class KLCommandRegistry : public CommandRegistry
{
  /**
    KLCommandRegistry specialized CommandRegistry by having a reference to the KL command
    registry. It can create C++ wrappers (see KLCommand and KLScriptableCommand) of any KL 
    command registered in KL. Because there is no callback from KL to C++, the method 
    'synchronizeKL' must be called from C++ to synchronize the registries.
  */  
  Q_OBJECT

  public:
    // Command type
    QString COMMAND_KL; 
 
    KLCommandRegistry(
      FabricCore::Client client
      );

    virtual ~KLCommandRegistry();

    /// Implementation of CommandRegistry.
    virtual Command* createCommand(
      const QString &cmdName
      );

    /// Gets the KL registry.
    FabricCore::RTVal getKLRegistry();

    /// Gets the FabricCore client.
    virtual FabricCore::Client getClient();
  
  public slots:
    /// Synchronizes with the KL registry.
    /// Allows to create KL command from C++/Python.
    void synchronizeKL();
    
  private:
    /// Implementation of CommandRegistry.
    /// Registers the commands as a KL AppCommand in  
    /// the KL registry so it can be called from KL.
    virtual void commandIsRegistered(
      const QString &cmdName,
      const QString &cmdType,
      const QString &implType
    );

    /// Registers a KL command so it can be created from C++/Python
    /// Automatically called when synchronizing the registries.
    void registerKLCommand(
      const QString &cmdName
      );

    /// Creates a C++ KLCommand or KLSriptableCommand wrapper 
    /// for a KL command registered in the KL KLCommandRegistry.
    Command* createKLCommand(
      const QString &cmdName
      );

    /// Fabric client.
    FabricCore::Client m_client;
    /// KL command registry.
    FabricCore::RTVal m_klCmdRegistry;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_REGISTRY__
