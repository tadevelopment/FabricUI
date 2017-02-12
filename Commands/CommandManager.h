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
  */

  public:
    CommandManager();

    virtual ~CommandManager();

    /// Implementation of BaseCommandManager
    BaseCommand* createCommand(
      const QString &cmdName, 
      const QMap<QString, QString> &args = QMapString(), 
      bool doCmd = true);
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMMAND_MANAGER__
