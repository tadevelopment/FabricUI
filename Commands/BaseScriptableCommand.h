//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SCRIPTABLE_COMMAND__
#define __UI_BASE_SCRIPTABLE_COMMAND__

#include <QMap>
#include "BaseCommand.h"
#include "ScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class BaseScriptableCommand 
  : public BaseCommand
  , public ScriptableCommand
{
  /**
    BaseScriptableCommand is a base implementation of ScriptableCommand.
    New scriptable commands can inheritated this class.

    C++ interfaces cannot be wrapped in pyhton by shiboken.
    If you want your command to be accessible from python,
    it must derived from this class.
  */

  public:
    BaseScriptableCommand();
    
    virtual ~BaseScriptableCommand();

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

    /// List of arguments {argName, argValue}
    QMap<QString, QString> m_args;
    
  private:    
    /// Defines the arguments specs:
    /// default value, optional
    struct ScriptableCommandArgSpec 
    {
      QString type;
      bool optional;
      QString defaultValue;
    };

    /// List of arguments specs {argName, {defaultValue, optional}}
    QMap<QString, ScriptableCommandArgSpec> m_argSpecs;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_SCRIPTABLE_COMMAND__
