//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_KL_COMMAND_REGISTRY_PYTHON__
#define __UI_KL_COMMAND_REGISTRY_PYTHON__

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
    KLCommandRegistry_Python(
      FabricCore::Client client
      );

    virtual ~KLCommandRegistry_Python();

  protected:
    /// Wraps CommandRegistry::createCommand.
    /// To override in Python
    virtual BaseCommand* _createCommand_Python(
      const QString &cmdName
      );

  private:
    /// Implementation of CommandRegistry, 
    /// calls the Python version.
    virtual Command* createCommand(
      const QString &cmdName
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_KL_COMMAND_REGISTRY_PYTHON__
