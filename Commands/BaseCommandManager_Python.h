//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_COMMAND_MANAGER_PYTHON__
#define __UI_BASE_COMMAND_MANAGER_PYTHON__

#include "BaseCommandManager.h"

namespace FabricUI {
namespace Commands {

class BaseCommandManager_Python : protected BaseCommandManager
{
  /**
    Pyside/shiboken doesn't propagate C++ exceptions to Python, cf. https://bugreports.qt.io/browse/PYSIDE-62.
    BaseCommandManager_Python "wraps" methods of BaseCommandManager throwing C++ exceptions called
    from Python. The exceptions are catched and returned as strings ao they can be raised in Python.
  */

 	public:
    BaseCommandManager_Python();

    virtual ~BaseCommandManager_Python();

  protected:
    /// Wraps BaseCommandManager method.
    QString doCommand_Python(
    	BaseCommand *cmd
      );

    /// Wraps BaseCommandManager method.
    QString undoCommand_Python();

    /// Wraps BaseCommandManager method.
    QString redoCommand_Python();

    /// Wraps BaseCommandManager method.
    QString checkCommandArgs_Python(
      BaseCommand *cmd,
      const QMap<QString, QString> &args
      ) const;

    /// \internal
    int totalCountAtStackIndex() const;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_COMMAND_MANAGER_PYTHON__
