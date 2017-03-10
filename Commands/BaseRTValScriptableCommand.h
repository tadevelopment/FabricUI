//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__
#define __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__

#include <FabricCore.h>
#include "BaseScriptableCommand.h"

namespace FabricUI {
namespace Commands {

class BaseRTValScriptableCommand : public BaseScriptableCommand
{
  /**
    BaseRTValScriptableCommand complements BaseScriptableCommand
    with methods to declare and set/get the arguments with RTVal.  
  */

  public:
    BaseRTValScriptableCommand();
    
    virtual ~BaseRTValScriptableCommand();
 
    /// Declares and sets an argument.
    /// To call from the command constructor.
    void declareArg( 
      const QString &key, 
      const QString &type,
      bool optional = true, 
      const QString &defaultValue = QString()
      );

    /// Declares and sets an argument using RTVal.
    /// To call from the command constructor.
    void declareArg( 
      const QString &key, 
      const QString &type,
      bool optional, 
      FabricCore::RTVal defaultValue
      );

    /// Sets the type of an argument.
    /// Used the the arg type is known
    /// when the command is executed.
    void setArgType(
      const QString &key, 
      const QString &type
      );
 
    /// Gets the arguments as RTVals.
    QMap<QString, FabricCore::RTVal> getArgsAsRTVal();

    /// Gets the value of an argument as RTVal.
    FabricCore::RTVal getArgAsRTVal( 
      const QString &key 
      );

    /// Implementation of BaseScriptableCommand.
    virtual QString getDescription();
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_RTVAL_SCRIPTABLE_COMMAND__
