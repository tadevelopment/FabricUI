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
  */

  public:
    virtual ~RTValScriptableCommand() {}
    
    /// Declares an argument, called from constructor.
    /// \param key Argument key
    /// \param type RTVal type 
    /// \param optional If true, default.
    /// \param defaultValue JSON definition of the RTVal
    virtual void declareRTValArg( 
      const QString &key, 
      const QString &type,
      bool optional, 
      const QString &defaultValue
      ) = 0;

    /// Declares an argument, called from constructor.
    /// \param key Argument key
    /// \param type RTVal type 
    /// \param optional If true, default.
    /// \param defaultValue Default value 
    virtual void declareRTValArg( 
      const QString &key, 
      const QString &type,
      bool optional, 
      FabricCore::RTVal defaultValue
      ) = 0;

    /// Gets the argument rtval type.
    /// \param key Argument key
    virtual QString getRTValArgType(
      const QString &key
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
