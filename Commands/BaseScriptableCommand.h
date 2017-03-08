//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SCRIPTABLE_COMMAND__
#define __UI_BASE_SCRIPTABLE_COMMAND__

#include <QMap>
#include "BaseCommand.h"

namespace FabricUI {
namespace Commands {

class BaseScriptableCommand : public BaseCommand
{
  /**
    BaseScriptableCommand defines the methods for commands with arguments.
    Any scriptable command must inherites from this class. Each argument
    is identified by a key. 

    Because the arguments have to be set from any app/DCCs, the args values
    are passed as QString only. However, the QString class provides helpers
    to cast QString to basic types, see http://doc.qt.io/qt-4.8/qstring.html.
    Another possibility is to use JSON format, see KLScriptableCommand.
  */

  public:
    BaseScriptableCommand();
    
    virtual ~BaseScriptableCommand();

    /// Declares and sets an argument.
    /// To call from the command constructor.
    void declareArg( 
      const QString &key, 
      bool optional = true, 
      const QString &defaultValue = QString()
      );

    /// Gets the arguments.
    virtual QMap<QString, QString> getArgs();

    /// Gets the value of an argument.
    virtual QString getArg( 
      const QString &key 
      );

    /// Sets the value of an argument.
    /// Throws an error if the key is 
    /// empty or has't been declared.
    virtual void setArg(
      const QString &key, 
      const QString &value
      );

    /// Check if the arguments are correctly set. 
    /// Throws an error if not.
    virtual void validateSetArgs();

    /// Gets a description of the command
    /// logged in the script-editor:
    /// cmdName(arg1:val1, arg2:val2, ...)
    virtual QString getDescription();

    /// Gets a decription of the arguments.
    /// Used for debugging.
    virtual QString getArgsDescription();

  protected:
    /// Defines the arguments specs:
    /// default value, optional
    struct ScriptableCommandArgSpec 
    {
      QString type;
      QString defaultValue;
      bool optional;
    };

    /// List of arguments {argName, argValue}
    QMap<QString, QString> m_args;
    /// List of arguments specs {argName, {defaultValue, optional}}
    QMap<QString, ScriptableCommandArgSpec> m_argSpecs;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_SCRIPTABLE_COMMAND__
