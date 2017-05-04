//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__
#define __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__

#include "BaseScriptableCommand.h"
#include "RTValScriptableCommand.h"

namespace FabricUI {
namespace Commands {

// Need to derived from BaseScriptableCommand for Python.
class BaseRTValScriptableCommand 
  : public BaseScriptableCommand 
  , public RTValScriptableCommand
{
  /**
    BaseRTValScriptableCommand is a base implementation of RTValScriptableCommand.
    New rtval scriptable commands can inheritated this class.

    C++ interfaces cannot be wrapped in pyhton by shiboken.
    If you want your command to be accessible from python,
    it must derived from this class.
  */

  public:
    BaseRTValScriptableCommand();
    
    virtual ~BaseRTValScriptableCommand();
    
    /// Implementation of ScriptableCommand.
    virtual void declareArg( 
      const QString &key, 
      bool optional = true, 
      const QString &defaultValue = QString()
      );

    /// Implementation of ScriptableCommand.
    virtual bool hasArg(
      const QString &key 
      );

    /// Implementation of ScriptableCommand.
    virtual QList<QString> getArgKeys();

    /// Implementation of ScriptableCommand.
    virtual QString getArg( 
      const QString &key 
      );
 
    /// Implementation of ScriptableCommand.
    /// Throws an error if the key is empty 
    /// or has't been declared.
    virtual void setArg(
      const QString &key, 
      const QString &value
      );

    /// Implementation of ScriptableCommand.
    /// Throws an error if not valid.
    virtual void validateSetArgs();

    /// Implementation of ScriptableCommand.
    virtual QString getArgsDescription();

    /// Implementation of RTValScriptableCommand.
    virtual void declareRTValArg( 
      const QString &key, 
      const QString &type,
      bool optional = true, 
      const QString &defaultValue = QString()
      );

    /// Implementation of RTValScriptableCommand.
    virtual void declareRTValArg( 
      const QString &key, 
      const QString &type,
      bool optional, 
      FabricCore::RTVal defaultValue
      );

    /// Implementation of RTValScriptableCommand.
    virtual QString getRTValArgType(
      const QString &key
      );

    /// Implementation of RTValScriptableCommand.
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key 
      );

    /// Implementation of RTValScriptableCommand.
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key,
      const QString &type
      );

    /// Implementation of RTValScriptableCommand.
    virtual void setRTValArg(
      const QString &key, 
      FabricCore::RTVal value
      );

  private:    
    /// Defines the arguments specs:
    /// type, default value, optional
    struct ScriptableCommandRTValArgSpec 
    {
      QString type;
      bool optional;
      FabricCore::RTVal defaultValue;
    };

    /// List of arguments {argName, argValue}
    QMap<QString, FabricCore::RTVal> m_rtvalArgs;
    /// List of arguments specs {argName, {defaultValue, optional}}
    QMap<QString, ScriptableCommandRTValArgSpec> m_rtvalArgSpecs;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__
