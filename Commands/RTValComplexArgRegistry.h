//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_COMPLEX_ARG_REGISTRY__
#define __UI_COMPLEX_ARG_REGISTRY__

#include <QList>
#include "RTValComplexArg.h"

namespace FabricUI {
namespace Commands {

class RTValComplexArgRegistry {
  /**
    RTValComplexArgRegistry registers RTValComplexArgs. 
    The args  are retreived by the flags used to create them.
  */

  public:
    RTValComplexArgRegistry();
        
    ~RTValComplexArgRegistry();

    /// Registers a RTValComplexArg.
    void registerArg(
      RTValComplexArg *arg
      );

    /// Gets a RTValComplexArg.
    RTValComplexArg* getRTValComplexArg(
      int flags
      );

    /// \internal,
    bool getRTValComplexArgType(
      int flags,
      QString &type
      );

    /// \internal
    QString setRTValComplexArgValue(
      int flags,
      QString key, 
      FabricCore::RTVal value, 
      FabricCore::RTVal &pathValue
      );

    /// \internal
    QString getRTValComplexArgValueType(
      int flags,
      QString key, 
      QString type
      );

    /// \internal
    FabricCore::RTVal getRTValComplexArgValue(
      int flags,
      QString key,
      FabricCore::RTVal pathValue
      );

    /// \internal
    bool isRTValComplexArgTypeOf(
      int flags,
      const QString &json
      );

  private:
    /// \internal
    QList<RTValComplexArg *> m_registeredArgs;
    
};

} // namespace Commands
} // namespace FabricUI

#endif // __UI_COMPLEX_ARG_REGISTRY__
