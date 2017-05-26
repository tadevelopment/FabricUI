//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SCRIPTABLE_COMMAND__
#define __UI_BASE_SCRIPTABLE_COMMAND__

#include <QMap>
#include "BaseCommand.h"
#include "CommandArgFlags.h"

namespace FabricUI {
namespace Commands {

class BaseScriptableCommand : public BaseCommand
{
  /**
    BaseScriptableCommand defines methods for commands with arguments. 
    Any scriptable command must implements this interface class.

    Because the arguments must be scriptable (to work with any app/DCC),
    they must rely on string (QString here). QString provides helpers to   
    cast string to others types, see http://doc.qt.io/qt-4.8/qstring.html.
    Another possibility is to use JSON format, see BaseRTValScriptableCommand.
    Arguments are set from within the CommandManager, method 'createCommand'.

    Arguments can be declared:
      - as optional: The arg not need to be set
      - as loggable: If true, the argument
      - with a default argument

    C++ interfaces cannot be wrapped in python by shiboken. New commands
    must specialize this class to be accessible from python.
  */
    
  Q_OBJECT

  public:
    BaseScriptableCommand();
    
    virtual ~BaseScriptableCommand();

    /// Declares an argument, called from constructor.
    /// \param key Argument key
    /// \param optional If true, default.
    /// \param defaultValue Default value 
    /// \param loggable If true, the arg is logged in the script-editor. 
    virtual void declareArg( 
      const QString &key, 
      int flag, 
      const QString &defaultValue
      );

    /// Checks if the command has an arg.
    /// \param key Argument key
    virtual bool hasArg(
      const QString &key 
      );

    /// Checks if an arg is optional.
    /// \param key Argument key
    virtual bool isArg(
      const QString &key,
      int flag
      );

    /// Gets the arguments keys.
    virtual QList<QString> getArgKeys();

    /// Checks if an arg has been set.
    /// To get safely optional argument.
    virtual bool isArgSet(
      const QString &key
      );

    /// Gets an argument.
    /// \param key Argument key
    virtual QString getArg( 
      const QString &key 
      );
 
    /// Sets an argument, called from the manager.
    /// \param key Argument key
    /// \param value The new arg.
    virtual void setArg(
      const QString &key, 
      const QString &value
      );

    /// Checks the args are correctly set before  
    /// doing the command, called from the manager.
    virtual void validateSetArgs();

    /// Gets a decription of the args, for debugging.
    virtual QString getArgsDescription();

  private:    
    /// Defines the arguments specs:
    /// default value, flags (optional, loggable)
    struct ScriptableCommandArgSpec 
    {
      int flags;
      QString defaultValue;
    };

    /// List of arguments {argName, argValue}
    QMap<QString, QString> m_args;
  
    /// List of arguments specs {argName, spec}
    QMap<QString, ScriptableCommandArgSpec> m_argSpecs;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_SCRIPTABLE_COMMAND__
