//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_COMMAND_MANAGER__
#define __UI_RTVAL_COMMAND_MANAGER__
 
#include <FabricCore.h>
#include "CommandManager.h"
 
namespace FabricUI {
namespace Commands {

class RTValCommandManager : public CommandManager
{
  /**
    RTValCommandManager specializes CommandManager to support 
    RTValScriptable commands.
  */
  Q_OBJECT

  public:
    RTValCommandManager(
      FabricCore::Client client
      );

    virtual ~RTValCommandManager();
    
    /// Creates and executes a command (if doCmd == true).
    /// If executed, the command is added to the manager stack.
    /// Throws an exception if an error occurs.
    virtual Command* createRTValCommand(
      const QString &cmdName, 
      const QMap<QString, FabricCore::RTVal> &args, 
      bool doCmd = true
      );;
    
    /// Gets the Fabric client.
    virtual FabricCore::Client getClient();
    
  protected:
    /// Checks the command arguments before doing it.
    /// Throws an exception if an error occurs.
    virtual void checkRTValCommandArgs(
      Command *cmd,
      const QMap<QString, FabricCore::RTVal> &args
      );

    /// Fabric client.
    FabricCore::Client m_client;
};
 
} // namespace Commands
} // namespace FabricUI

#endif // __UI_RTVAL_COMMAND_MANAGER__
