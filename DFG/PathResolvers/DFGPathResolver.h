//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_PATH_RESOLVER__
#define __UI_DFG_PATH_RESOLVER__

#include <QMap>
#include <FabricUI/PathResolvers/PathResolver.h>

namespace FabricUI {
namespace DFG {

class DFGPathResolver : public PathResolvers::PathResolver
{
  /**
    DFGPathResolver  
  */  
 
  public:
    DFGPathResolver(
      FabricCore::Client client
      );

    virtual ~DFGPathResolver();
 
    void setBindingID(
      const QString &path,
      int bindingID
      );
  
    /// Implementation of PathResolver.
    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );

    /// Implementation of PathResolver.
    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    /// Implementation of PathResolver.
    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    /// Implementation of PathResolver.
    virtual void setValue(
      FabricCore::RTVal pathValue
      );

  private:
    /// 
    FabricCore::Client m_client;
    ///
    FabricCore::DFGBinding m_binding;
    ///
    QMap<QString, int> m_pathIDMap;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_PATH_RESOLVER__
