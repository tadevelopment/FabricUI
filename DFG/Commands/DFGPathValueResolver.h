//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_PATH_VALUE_RESOLVER__
#define __UI_DFG_PATH_VALUE_RESOLVER__

#include <FabricUI/Commands/BasePathValueResolver.h>

namespace FabricUI {
namespace DFG {

class DFGPathValueResolver : public Commands::BasePathValueResolver
{
  /**
    DFGPathValueResolver specialize for DFG graphs. 
  */  
  
  Q_OBJECT
  
  public:
    DFGPathValueResolver();
 
    virtual ~DFGPathValueResolver();
   
    /// Implementation of BasePathValueResolver.
    virtual void registrationCallback(
      QString const&name,
      void *userData
      );

    /// Implementation of BasePathValueResolver.
    virtual bool knownPath(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual QString getType(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual void getValue(
      FabricCore::RTVal pathValue
      );

    /// Implementation of BasePathValueResolver.
    virtual void setValue(
      FabricCore::RTVal pathValue
      );

    /// Gets the executable and the relative
    /// port path from `pathValue`
    FabricCore::DFGExec getSubExecAndPortPath(
      FabricCore::RTVal pathValue, 
      QString &relPortPath
      );

  public slots:
    /// Update the binding.
    virtual void onBindingChanged(
      FabricCore::DFGBinding const &binding
      );

  private:
    /// Gets a port/var relative path.
    QString getRelativePath(
      FabricCore::RTVal pathValue
      );

    /// Type of DFG data.
    enum DFGType { DFGUnknow, DFGPort, DFGBlockPort, DFGArg, DFGVar };
      
    struct DFGPortPaths {
      QString portName; // Relative
      QString blockName;
      QString nodeName;
      QString subExecPath;

      DFGPortPaths() {
       // isBlockPort = false;
      }

      bool isExecBlockPort() {
        return !blockName.isEmpty();
      }

      QString getRelativePortPath() {
        if(isExecBlockPort())
          return nodeName + "." + blockName + "." + portName;
        else if(isExecArg())
          return portName;
        else
          return nodeName + "." + portName;
      }

      bool isExecArg() {
        return !isExecBlockPort() && nodeName.isEmpty();
      }
    };

    /// Gets the executable and the relative
    /// port path from `pathValue`
    FabricCore::DFGExec getSubExecAndPortPaths(
      FabricCore::RTVal pathValue, 
      DFGPortPaths &portPaths
      );

    /// Gets the DFG data type (DFGUnknow, DFGPort, DFGVar)
    DFGType getDFGType(
      FabricCore::RTVal pathValue
      );  

    /// Casts the path the a humain readable format.
    void castPathToHRFormat(
      FabricCore::RTVal pathValue
      );

    QString m_id;
    FabricCore::DFGBinding m_binding;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_PATH_VALUE_RESOLVER__
