//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_COMMAND_MANAGER_PYTHON__
#define __UI_BASE_COMMAND_MANAGER_PYTHON__

#include "CommandManager.h"

namespace FabricUI {
namespace Commands {

class CommandManager_Python : public CommandManager
{
  /**
    Pyside/shiboken doesn't propagate C++ exceptions to Python, cf. https://bugreports.qt.io/browse/PYSIDE-62.
    CommandManager_Python "wraps" methods of CommandManager throwing C++ exceptions called
    from Python. The exceptions are catched and returned as strings ao they can be raised in Python.
  */

 	public:
    CommandManager_Python(
      FabricCore::Client client
      );

    virtual ~CommandManager_Python();

  protected:
    /// Wraps CommandManager method.
    QString doCommand_Python(
    	BaseCommand *cmd
      );

    /// Wraps CommandManager method.
    QString undoCommand_Python();

    /// Wraps CommandManager method.
    QString redoCommand_Python();

    /// Wraps CommandManager method.
    QString checkCommandArgs_Python(
      BaseCommand *cmd,
      const QMap<QString, QString> &args
      );

    /// Gets the total number of commands
    /// (low+top) at the current index.
    /// Used by the python command manager.
    int totalCountAtStackIndex();
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_COMMAND_MANAGER_PYTHON__
