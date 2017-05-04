//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_SCRIPTABLE_COMMAND__
#define __UI_SCRIPTABLE_COMMAND__

#include <QList>
#include <QString>
 
namespace FabricUI {
namespace Commands {

class ScriptableCommand
{
  /**
    ScriptableCommand defines methods for commands with arguments. 
    Any scriptable command must implements this interface class.

    Because the arguments have to be set from any app/DCCs, they are 
    passed as QString only. However, QString provides helpers to cast 
    string to others types, see http://doc.qt.io/qt-4.8/qstring.html.
    Another possibility is to use JSON format, see RTValScriptableCommand.
  */

  public:
    /// Declares an argument, called from constructor.
    /// \param key Argument key
    /// \param optional If true, default.
    /// \param defaultValue Default value 
    virtual void declareArg( 
      const QString &key, 
      bool optional, 
      const QString &defaultValue
      ) = 0;

    /// Checks if a command has an arg.
    /// \param key Argument key
    virtual bool hasArg(
      const QString &key 
      ) = 0;

    /// Gets the arguments keys.
    virtual QList<QString> getArgKeys() = 0;

    /// Gets an argument.
    /// \param key Argument key
    virtual QString getArg( 
      const QString &key 
      ) = 0;
 
    /// Sets an argument.
    /// \param key Argument key
    /// \param value The new arg.
    virtual void setArg(
      const QString &key, 
      const QString &value
      ) = 0;

    /// Checks the args are correctly set before  
    /// doing the command, called from the manager.
    virtual void validateSetArgs() = 0;

    /// Gets a decription of the arguments.
    /// Used for debugging.
    virtual QString getArgsDescription() = 0;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_SCRIPTABLE_COMMAND__
