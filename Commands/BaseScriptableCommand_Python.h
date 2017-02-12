//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SCRIPABLE_COMMAND_PYTHON__
#define __UI_BASE_SCRIPABLE_COMMAND_PYTHON__

#include "BaseScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class BaseScriptableCommand_Python : protected BaseScriptableCommand
{
  /**
    Pyside/shiboken doesn't propagate C++ exceptions to Python, cf. https://bugreports.qt.io/browse/PYSIDE-62.
    BaseScriptableCommand_Python "wraps" the methods of BaseScriptableCommand that throws C++ exceptions.
    The exceptions are catched and returned as strings ao they can be raised in Python.
  */

  public:
    BaseScriptableCommand_Python();
    
    virtual ~BaseScriptableCommand_Python();

  protected:
    /// Wraps BaseScriptableCommand method.
    QString declareArg_Python( 
      const QString &key, 
      bool optional = true, 
      const QString &defaultValue = QString()
      );

    /// Wraps BaseScriptableCommand method.
    QString setArg_Python(
      const QString &key, 
      const QString &value
      );

    /// Wraps BaseScriptableCommand method.
    QString validateSetArgs_Python() const;
 
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_SCRIPABLE_COMMAND_PYTHON__
