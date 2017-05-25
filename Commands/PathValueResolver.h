//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_PATH_VALUE_RESOLVER__
#define __UI_PATH_VALUE_RESOLVER__

#include <QString>
#include <FabricCore.h>
 
namespace FabricUI {
namespace Commands {

class PathValueResolver
{
  /**
    PathValueResolver  
  */  
 
  public:
    virtual ~PathValueResolver() {}

    virtual void registrationCallback(
      void *userData
      ) = 0;

    virtual bool knownPath(
      FabricCore::RTVal pathValue
      ) = 0;

    virtual QString getType(
      FabricCore::RTVal pathValue
      ) = 0;

    virtual void getValue(
      FabricCore::RTVal pathValue
      ) = 0;

    virtual void setValue(
      FabricCore::RTVal pathValue
      ) = 0;
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_PATH_VALUE_RESOLVER__
