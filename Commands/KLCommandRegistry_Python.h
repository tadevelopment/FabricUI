//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND_REGISTRY_PYTHON__
#define __UI_KL_COMMAND_REGISTRY_PYTHON__

#include <QPair>
#include "BaseCommand.h"
#include "KLCommandRegistry.h"

namespace FabricUI {
namespace Commands {

class KLCommandRegistry_Python : public KLCommandRegistry
{
  /**
    Pyside/shiboken doesn't propagate C++ exceptions to Python, cf. https://bugreports.qt.io/browse/PYSIDE-62.
    KLCommandRegistry_Python "wraps" methods of KLCommandRegistry throwing C++ exceptions called from Python.
    The exceptions are catched and returned as strings ao they can be raised in Python.

    In addition, it 'redirects' methods of KLCommandRegistry that expose the 'Command' interfaces because C++ 
    interfaces cannot be wrapped in pyhton by shiboken.
  */
  Q_OBJECT
  
 	public:
    KLCommandRegistry_Python();

    virtual ~KLCommandRegistry_Python();

  protected:
    // Python -> C++

    /// Wraps CommandRegistry::createCommand.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QPair<QString, BaseCommand*> _createCommand_Python(
      const QString &cmdName
      );

    /// Wraps CommandRegistry::commandIsRegistered.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _commandIsRegistered_Python(
      const QString &cmdName,
      const QString &cmdType,
      const QString &implType
      );

    /// Wraps KLCommandRegistry::synchronizeKL.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _synchronizeKL_Python();

  private:
    // C++ -> Python

    /// Implementation of CommandRegistry, 
    /// calls _createCommand_Python.
    virtual Command* createCommand(
      const QString &cmdName
      );

    /// Implementation of CommandRegistry, 
    /// calls _commandIsRegistered_Python.
    virtual void commandIsRegistered(
      const QString &cmdName,
      const QString &cmdType,
      const QString &implType
      );

    /// Implementation of KLCommandRegistry.
    /// calls _synchronizeKL_Python.
    virtual void synchronizeKL();
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_REGISTRY_PYTHON__
