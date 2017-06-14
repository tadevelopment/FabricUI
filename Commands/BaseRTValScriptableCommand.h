//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__
#define __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__

#include <QPair>
#include <FabricCore.h>
#include "CommandArgHelpers.h"
#include "BaseScriptableCommand.h"
 
namespace FabricUI {
namespace Commands {

// Need to derived from BaseScriptableCommand for Python.
class BaseRTValScriptableCommand : public BaseScriptableCommand 
{
  /**
    BaseRTValScriptableCommand is a base implementation of BaseRTValScriptableCommand.
    New RTVal scriptable commands can specialize this class.
    
    C++ RTVal are generic C++ wrappers for KL objects/structure
    that can be reprensented in a JSON format. 

    Because RTVal can be represented as JSON (string), JSON can be used to set
    an argument. We use this feature if the type of the RTVal is not known
    when the argument is declared and the command is executed from the script-
    editor.  

    BaseRTValScriptableCommand support by-default RTValPathValueArg so it can 
    get/set RTVal of DFG ports or attributes using their paths.

    C++ interfaces cannot be wrapped in python by shiboken. New commands
    must specialize this class to be accessible from python.
  */
  Q_OBJECT
  
  public:
    BaseRTValScriptableCommand();
    
    virtual ~BaseRTValScriptableCommand();
    
    /// Implementation of BaseScriptableCommand.
    /// Declares an arg. of unknown RTVal type.
    /// The parameter 'defaultValue' does nothing.
    /// To set the argument as JSON, use `setArg`.
    virtual void declareArg( 
      QString key, 
      int flags = FabricUI::Commands::CommandArgFlags::NO_FLAG_ARG, 
      QString defaultValue = QString()
      );

    /// Implementation of BaseScriptableCommand.
    virtual bool hasArg(
      QString key 
      );

    /// Implementation of BaseScriptableCommand.
    virtual bool isArg(
      QString key,
      int flag
      );
 
    /// Implementation of BaseScriptableCommand.
    virtual QList<QString> getArgKeys();

    /// Implementation of BaseScriptableCommand.
    virtual bool isArgSet(
      QString key
      );
    
    /// Implementation of BaseScriptableCommand.
    /// Gets the JSON representation of the arg.
    virtual QString getArg( 
      QString key 
      );
 
    /// Implementation of BaseScriptableCommand.
    /// Sets a JSON representation of the arg.
    /// To use when the RTVal type of the arg
    /// is unknown (e.g set from the script-editor).
    /// Throws an error if the key is empty or 
    /// hasn't been declared.
    virtual void setArg(
      QString key, 
      QString value
      );

    /// Implementation of BaseScriptableCommand.
    /// Validates that the args've been set
    /// either as RTVal or as JSON.
    /// Throws an error if not valid.
    virtual void validateSetArgs();

    /// Implementation of BaseScriptableCommand.
    virtual QString getArgsDescription();

    /// Declares an argument, called from constructor.
    /// \param key Argument key
    /// \param type RTVal type 
    /// \param optional If true, the arg is  optional.
    /// \param defaultValue Default value 
    /// \param loggable If true, the arg is logged in the script-editor.
    /// Declares an argument of known RTVal type.
    virtual void declareRTValArg( 
      QString key, 
      QString type,
      int flags = FabricUI::Commands::CommandArgFlags::NO_FLAG_ARG, 
      FabricCore::RTVal defaultValue = FabricCore::RTVal()
      );

    /// Gets the argument rtval type.
    virtual QString getRTValArgType(
      QString key
      );
    
    /// Gets an argument.
    virtual QString getRTValArgPath( 
      QString key 
      );

    /// Gets an argument.
    virtual FabricCore::RTVal getRTValArgValue( 
      QString key 
      );

    /// Gets an argument if type not been set.
    /// To use with unknown type argument. The RTVal 
    /// is constructed from the JSON and the type
    virtual FabricCore::RTVal getRTValArgValue( 
      QString key,
      QString type
      );

    /// Sets an argument.
    virtual void setRTValArgValue(
      QString key, 
      FabricCore::RTVal value
      );

    virtual void setRTValArg(
      QString key, 
      FabricCore::RTVal pathValue
      );

    virtual FabricCore::RTVal getRTValArg(
      QString key 
      );

  protected:
    void checkHasArg(
      QString methodName,
      QString key
      );
      
    /// Checks if the argument is a PathValue.
    bool isJSONPathValueArg(
      QString json
      );

  private:
    bool isArgTypeKnown(
      QString key
      );

    void checkEmptyKey(
      QString methodName,
      QString key
      );

    void checkRTValType(
      QString methodName,
      QString key,
      QString type
      );

    /// Defines the arguments specs:
    /// type, default value, flags (optional, loggable)
    struct ScriptableCommandRTValArgSpec 
    {
      QString type;
      int flags;
      FabricCore::RTVal defaultValue;
    };

    /// List of arguments {argName, argValue}
    QMap<QString, QPair<FabricCore::RTVal, QString> > m_rtvalArgs;
    /// List of arguments specs {argName, spec}
    QMap<QString, ScriptableCommandRTValArgSpec> m_rtvalArgSpecs;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__
