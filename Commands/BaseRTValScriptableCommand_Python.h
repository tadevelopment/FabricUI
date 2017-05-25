//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SCRIPTABLE_COMMAND_PYTHON__
#define __UI_BASE_SCRIPTABLE_COMMAND_PYTHON__

#include <QPair>
#include "BaseRTValScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class BaseRTValScriptableCommand_Python : public BaseRTValScriptableCommand
{
  /**
    Pyside/shiboken doesn't propagate C++ exceptions to Python, cf. https://bugreports.qt.io/browse/PYSIDE-62.
    BaseRTValScriptableCommand_Python "wraps" methods of BaseRTValScriptableCommand throwing C++ exceptions called from Python.
    The exceptions are catched and returned as strings ao they can be raised in Python.
  */  
 	public:
    BaseRTValScriptableCommand_Python();

    virtual ~BaseRTValScriptableCommand_Python();

  protected:
    // Python -> C++
 
    /// Wraps BaseRTValScriptableCommand::declareArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _declareArg_Python(
      const QString &key, 
      int flag, 
      const QString &defaultValue
      );

    /// Wraps BaseRTValScriptableCommand::isArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QPair<QString, bool> _isArg_Python(
      const QString &key,
      int flag);
 
    /// Wraps BaseRTValScriptableCommand::getArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QPair<QString, QString> _getArg_Python(
      const QString &key);

    /// Wraps BaseRTValScriptableCommand::setArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _setArg_Python(
      const QString &key,
      const QString &value);

    /// Wraps BaseRTValScriptableCommand::validateSetArgs.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _validateSetArgs_Python();

    /// Wraps BaseRTValScriptableCommand::declareRTValArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _declareRTValArg_Python( 
      const QString &key, 
      const QString &type,
      int flag, 
      FabricCore::RTVal defaultValue
      );

    /// Wraps BaseRTValScriptableCommand::getRTValArgType.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QPair<QString, QString> _getRTValArgType_Python(
      const QString &key
      );

    /// Wraps BaseRTValScriptableCommand::setRTValArgType.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _setRTValArgType_Python(
      const QString &key,
      const QString &type
      );

    /// Wraps BaseRTValScriptableCommand::getRTValArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QPair<QString, FabricCore::RTVal> _getRTValArg_Python( 
      const QString &key
      );

    /// Wraps BaseRTValScriptableCommand::getRTValArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QPair<QString, FabricCore::RTVal> _getRTValArg_Python( 
      const QString &key,
      const QString &type
      );

    /// Wraps BaseRTValScriptableCommand::setRTValArg.
    /// Propagates the C++ exception in python.
    /// To override in Python.
    virtual QString _setRTValArg_Python(
      const QString &key,
      FabricCore::RTVal value
      );

  private:
    // C++ -> Python
 
    /// Implementation of ScriptableCommand, 
    /// calls _declareArg_Python.
    virtual void declareArg(
      const QString &key, 
      int flag, 
      const QString &defaultValue
      );

    /// Implementation of ScriptableCommand.
    /// calls _isArg_Python.
    virtual bool isArg(
      const QString &key,
      int flag);
 
    /// Implementation of ScriptableCommand.
    /// calls _getArg_Python.
    virtual QString getArg(
      const QString &key);

    /// Implementation of ScriptableCommand.
    /// calls _setArg_Python.
    virtual void setArg(
      const QString &key,
      const QString &value);

    /// Implementation of ScriptableCommand.
    /// calls _validateSetArgs_Python.
    virtual void validateSetArgs();

    /// Implementation of RTValScriptableCommand.
    /// calls _declareRTValArg_Python.
    virtual void declareRTValArg( 
      const QString &key, 
      const QString &type,
      int flag, 
      FabricCore::RTVal defaultValue
      );

    /// Implementation of RTValScriptableCommand.
    /// calls _getRTValArgType_Python.
    virtual QString getRTValArgType(
      const QString &key
      );

    /// Implementation of RTValScriptableCommand.
    /// calls _setRTValArgType_Python.
    virtual void setRTValArgType(
      const QString &key,
      const QString &type
      );

    /// Implementation of RTValScriptableCommand.
    /// calls _getRTValArg_Python.
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key
      );

    /// Implementation of RTValScriptableCommand.
    /// calls _getRTValArg_Python.
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key,
      const QString &type
      );

    /// Implementation of RTValScriptableCommand.
    /// calls _setRTValArg_Python.
    virtual void setRTValArg(
      const QString &key,
      FabricCore::RTVal value
      );
  };

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_SCRIPTABLE_COMMAND_PYTHON__