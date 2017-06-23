//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_BASE_PATH_VALUE_RESOLVER__
#define __UI_BASE_PATH_VALUE_RESOLVER__

#include <QObject>
#include <QString>
#include <FabricCore.h>
 
namespace FabricUI {
namespace Commands {

class BasePathValueResolver : public QObject
{
  /**
    BasePathValueResolver  
  */  
  Q_OBJECT
  
  public:
    BasePathValueResolver();

    virtual ~BasePathValueResolver();

    /// Registration callback.
    virtual void registrationCallback(
      QString const&name,
      void *userData
      );
 
    /// Checks the resolvers knows the `PathValue` path.
    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );

    /// Gets the `PathValue` value type.
    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    /// Gets the `PathValue` value.
    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    /// Sets the `PathValue` value.
    virtual void setValue(
      FabricCore::RTVal pathValue
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_PATH_VALUE_RESOLVER__
