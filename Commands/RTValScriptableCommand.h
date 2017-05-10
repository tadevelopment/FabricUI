//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_RTVAL_SCRIPTABLE_COMMAND__
#define __UI_RTVAL_SCRIPTABLE_COMMAND__

#include <FabricCore.h>
 
namespace FabricUI {
namespace Commands {

class RTValScriptableCommand 
{
  /**
    RTValScriptableCommand complements ScriptableCommand 
    for RTVal based scriptable arguments.

    C++ RTVal are generic C++ wrappers for KL objects/structure
    that can be reprensented in a JSON format. 
  */

  public:
    virtual ~RTValScriptableCommand() {}
    
    /// Declares an argument, called from constructor.
    /// \param key Argument key
    /// \param type RTVal type 
    /// \param optional If true, the arg is  optional.
    /// \param defaultValue Default value 
    /// \param loggable If true, the arg is logged in the script-editor. 
    virtual void declareRTValArg( 
      const QString &key, 
      const QString &type,
      bool optional, 
      FabricCore::RTVal defaultValue,
      bool loggable
      ) = 0;

    /// Gets the argument rtval type.
    /// \param key Argument key
    virtual QString getRTValArgType(
      const QString &key
      ) = 0;

    /// Sets the argument type if not set.
    /// \param key Argument key
    /// \param type RTVal type 
    virtual void setRTValArgType(
      const QString &key,
      const QString &type
      ) = 0;

    /// Gets an argument.
    /// \param key Argument key
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key 
      ) = 0;

    /// Gets an argument if type not been set.
    /// \param key Argument key
    /// \param type RTVal type 
    virtual FabricCore::RTVal getRTValArg( 
      const QString &key,
      const QString &type
      ) = 0;

    /// Sets an argument.
    /// \param key Argument key
    /// \param value The new arg.
    virtual void setRTValArg(
      const QString &key,
      FabricCore::RTVal value
      ) = 0;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_RTVAL_SCRIPTABLE_COMMAND__
