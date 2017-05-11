//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__
#define __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__

#include "BaseScriptableCommand.h"
#include "RTValScriptableCommand.h"
#include "CommandFlags.h"

namespace FabricUI {
namespace Commands {

// Need to derived from BaseScriptableCommand for Python.
class BaseRTValScriptableCommand 
  : public BaseScriptableCommand 
  , public RTValScriptableCommand
{
  /**
    BaseRTValScriptableCommand is a base implementation of RTValScriptableCommand.
    New RTVal scriptable commands can specialize this class.
      
    Because RTVal can be represented as JSON (string), JSON can be used to set
    an argument. We use this feature if the type of the RTVal is not known
    when the argument is declared and the command is executed from the script-
    editor.  

    C++ interfaces cannot be wrapped in python by shiboken. New commands
    must specialize this class to be accessible from python.
  */

  public:
    BaseRTValScriptableCommand();
    
    virtual ~BaseRTValScriptableCommand();
    
    /// Implementation of ScriptableCommand.
    /// Declares an arg. of unknown RTVal type.
    /// The parameter 'defaultValue' does nothing.
    /// To set the argument as JSON, use `setArg`.
    virtual void declareArg( 
      const QString &key, 
      int flags = FabricUI::Commands::CommandFlags::LOGGABLE_ARG, 
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
    /// Gets the JSON representation of the arg.
    virtual QString getArg( 
      const QString &key 
      );
 
    /// Implementation of ScriptableCommand.
    /// Sets a JSON representation of the arg.
    /// To use when the RTVal type of the arg
    /// is unknown (e.g set from the script-editor).
    /// Throws an error if the key is empty or 
    /// hasn't been declared.
    virtual void setArg(
      const QString &key, 
      const QString &value
      );

    /// Implementation of ScriptableCommand.
    /// Validates that the args've been set
    /// either as RTVal or as JSON.
    /// Throws an error if not valid.
    virtual void validateSetArgs();

    /// Implementation of ScriptableCommand.
    virtual QString getArgsDescription();

    /// Implementation of RTValScriptableCommand.
    /// Declares an argument of known RTVal type.
    virtual void declareRTValArg( 
      const QString &key, 
      const QString &type,
      int flags = FabricUI::Commands::CommandFlags::LOGGABLE_ARG, 
      FabricCore::RTVal defaultValue = FabricCore::RTVal()
      );

    /// Implementation of RTValScriptableCommand.
    virtual QString getRTValArgType(
      const QString &key
      );

    /// Implementation of RTValScriptableCommand.
    /// To use with unknown type argument. The RTVal 
    /// is constructed from the JSON and the type
    virtual void setRTValArgType(
      const QString &key,
      const QString &type
      );

    /// Implementation of RTValScriptableCommand.
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key 
      );

    /// Implementation of RTValScriptableCommand.
    /// To use with unknown type argument. The RTVal 
    /// is constructed from the JSON and the type
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key,
      const QString &type
      );

    /// Implementation of RTValScriptableCommand.
    virtual void setRTValArg(
      const QString &key, 
      FabricCore::RTVal value
      );

  protected:
    /// Implementation of BaseScriptableCommand.
    virtual QString createHelpFromArgs(
      const QString &commandHelp,
      const QMap<QString, QString> &argsHelp
      );

  private:    
    /// Defines the arguments specs:
    /// type, default value, flags (optional, loggable)
    struct ScriptableCommandRTValArgSpec 
    {
      QString type;
      int flags;
      FabricCore::RTVal defaultValue;
    };

    /// List of arguments {argName, argValue}
    QMap<QString, FabricCore::RTVal> m_rtvalArgs;
    /// List of arguments specs {argName, spec}
    QMap<QString, ScriptableCommandRTValArgSpec> m_rtvalArgSpecs;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__
