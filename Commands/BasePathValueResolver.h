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

    virtual void registrationCallback(
      void *userData
      );

    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );

    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    virtual void setValue(
      FabricCore::RTVal pathValue
      );
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_BASE_PATH_VALUE_RESOLVER__
