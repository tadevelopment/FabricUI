//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_COMMAND_MANAGER__
#define __UI_RTVAL_COMMAND_MANAGER__
 
#include <FabricCore.h>
#include "CommandManager.h"
#include "RTValComplexArgRegistry.h"

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
    RTValCommandManager();

    virtual ~RTValCommandManager();
    
    /// Creates and executes a command (if doCmd == true).
    /// If executed, the command is added to the manager stack.
    /// Throws an exception if an error occurs.
    virtual BaseCommand* createCommand(
      const QString &cmdName, 
      const QMap<QString, FabricCore::RTVal> &args, 
      bool doCmd = true
      );

    /// Implementation of CommandManager.
    virtual void doCommand(
      BaseCommand* cmd
      );
    
    /// Gets the Fabric client.
    RTValComplexArgRegistry& getComplexArgRegistry();
  

  protected:
    /// Implementation of CommandManager.
    virtual void preProcessCommandArgs(
      BaseCommand* cmd
      );

    /// Implementation of CommandManager.
    virtual void postProcessCommandArgs(
      BaseCommand* cmd
      );

    /// Checks the command arguments before doing it.
    /// Throws an exception if an error occurs.
    virtual void checkCommandArgs(
      BaseCommand *cmd,
      const QMap<QString, FabricCore::RTVal> &args
      );

    RTValComplexArgRegistry m_RTValComplexArgRegistry;
};
 
} // namespace Commands
} // namespace FabricUI

#endif // __UI_RTVAL_COMMAND_MANAGER__
