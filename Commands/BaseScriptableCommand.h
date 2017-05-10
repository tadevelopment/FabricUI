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

    C++ interfaces cannot be wrapped in python by shiboken. New commands
    must specialize this class to be accessible from python.
  */

  public:
    BaseScriptableCommand();
    
    virtual ~BaseScriptableCommand();

    /// Implementation of ScriptableCommand.
    virtual void declareArg( 
      const QString &key, 
      bool optional = false, 
      const QString &defaultValue = QString(),
      bool loggable = true
      );

    /// Implementation of ScriptableCommand.
    virtual bool hasArg(
      const QString &key 
      );

    /// Implementation of ScriptableCommand.
    virtual bool isArgOptional(
      const QString &key 
      );

    /// Implementation of ScriptableCommand.
    virtual bool isArgLoggable(
      const QString &key 
      );

    /// Implementation of ScriptableCommand.
    virtual QList<QString> getArgKeys();

    /// Implementation of ScriptableCommand.
    virtual bool isArgSet(
      const QString &key
      );

    /// Implementation of ScriptableCommand.
    virtual QString getArg( 
      const QString &key 
      );
 
    /// Implementation of ScriptableCommand.
    /// Throws an error if the key is empty 
    /// or hasn't been declared.
    virtual void setArg(
      const QString &key, 
      const QString &value
      );

    /// Implementation of ScriptableCommand.
    /// Throws an error if not valid.
    virtual void validateSetArgs();

    /// Implementation of ScriptableCommand.
    /// Command name + list of all the arguments.
    virtual QString getArgsDescription();
    
    /// List of arguments {argName, argValue}
    QMap<QString, QString> m_args;
    
  protected:
    /// Helper to create the command's helps
    /// from a subsets of arguments.
    /// \param commandHelp The main help text.
    /// \param argsHelp Map of [arg, arg help]
    virtual QString createHelpFromArgs(
      const QString &commandHelp,
      const QMap<QString, QString> &argsHelp
      );

    /// Helper to create the command's desc
    /// from a subsets of arguments.
    /// \param argsHelp Map of [arg, arg desc]
    QString createHistoryDescFromArgs(
      const QMap<QString, QString> &argsDesc
      );

  private:    
    /// Defines the arguments specs:
    /// default value, optional, loggable
    struct ScriptableCommandArgSpec 
    {
      bool optional;
      bool loggable;  
      QString defaultValue;
    };

    /// List of arguments specs {argName, spec}
    QMap<QString, ScriptableCommandArgSpec> m_argSpecs;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_SCRIPTABLE_COMMAND__
