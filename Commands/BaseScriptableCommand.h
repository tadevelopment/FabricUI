//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_SCRIPTABLE_COMMAND__
#define __UI_BASE_SCRIPTABLE_COMMAND__

#include <QMap>
#include "BaseCommand.h"
#include "ScriptableCommand.h"
#include "CommandArgFlags.h"

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
      int flags = FabricUI::Commands::CommandArgFlags::LOGGABLE_ARG, 
      const QString &defaultValue = QString()
      );

    /// Implementation of ScriptableCommand.
    virtual bool hasArg(
      const QString &key 
      );

    /// Implementation of ScriptableCommand.
    virtual bool isArg(
      const QString &key,
      int flag 
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
