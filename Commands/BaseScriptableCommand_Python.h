//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SCRIPTABLE_COMMAND_PYTHON__
#define __UI_BASE_SCRIPTABLE_COMMAND_PYTHON__

#include <QPair>
#include "BaseScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class BaseScriptableCommand_Python : public BaseScriptableCommand
{
  /**
    Pyside/shiboken doesn't propagate C++ exceptions to Python, cf. https://bugreports.qt.io/browse/PYSIDE-62.
    BaseScriptableCommand_Python "wraps" methods of BaseScriptableCommand throwing C++ exceptions called from Python.
    The exceptions are catched and returned as strings ao they can be raised in Python.
  */  

  Q_OBJECT
  
 	public:
    BaseScriptableCommand_Python();

    virtual ~BaseScriptableCommand_Python();

  protected:
    // Python -> C++
 
    /// Wraps BaseScriptableCommand::declareArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _declareArg_Python(
      const QString &key, 
      int flag, 
      const QString &defaultValue
      );

    /// Wraps BaseScriptableCommand::isArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QPair<QString, bool> _isArg_Python(
      const QString &key,
      int flag);
 
    /// Wraps BaseScriptableCommand::getArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QPair<QString, QString> _getArg_Python(
      const QString &key);

    /// Wraps BaseScriptableCommand::setArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _setArg_Python(
      const QString &key,
      const QString &value);

    /// Wraps BaseScriptableCommand::validateSetArgs.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _validateSetArgs_Python();

  private:
    // C++ -> Python
 
    /// Implementation of BaseScriptableCommand, 
    /// calls _declareArg_Python.
    virtual void declareArg(
      const QString &key, 
      int flag, 
      const QString &defaultValue
      );

    /// Implementation of BaseScriptableCommand.
    /// calls _isArgOptional_Python.
    virtual bool isArg(
      const QString &key,
      int flag);
 
    /// Implementation of BaseScriptableCommand.
    /// calls _getArg_Python.
    virtual QString getArg(
      const QString &key);

    /// Implementation of BaseScriptableCommand.
    /// calls _setArg_Python.
    virtual void setArg(
      const QString &key,
      const QString &value);

    /// Implementation of BaseScriptableCommand.
    /// calls _validateSetArgs_Python.
    virtual void validateSetArgs();

  };

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_SCRIPTABLE_COMMAND_PYTHON__
