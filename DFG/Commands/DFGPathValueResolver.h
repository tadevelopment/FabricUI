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

    struct DFGPortPaths {
      QString portName; 
      QString blockName;
      QString nodeName;

      bool isExecBlockPort() {
        return !blockName.isEmpty();
      }

      bool isExecArg() {
        return !isExecBlockPort() && nodeName.isEmpty();
      }

      QString getRelativePortPath() {
        if(isExecBlockPort())
          return nodeName + "." + blockName + "." + portName;
        else if(isExecArg())
          return portName;
        else if(!nodeName.isEmpty())
          return nodeName + "." + portName;
        else
          return "";
      }
    };

    /// Gets the executable and DFGPortPaths
    /// from the pathValue.
    FabricCore::DFGExec getDFGPortPaths(
      FabricCore::RTVal pathValue, 
      DFGPortPaths &dfgPortPaths
      );

  public slots:
    /// Update the binding.
    virtual void onBindingChanged(
      FabricCore::DFGBinding const &binding
      );

  private:
    /// Removes the bindingID or the solverID from the path if set.
    QString getPathWithoutBindingOrSolverID(
      FabricCore::RTVal pathValue
      );

    /// Type of DFG item.
    enum DFGType { DFGUnknow, DFGPort, DFGArg, DFGVar };
      
    /// Gets the DFG item type (DFGUnknow, DFGPort, DFGVar)
    DFGType getDFGType(
      FabricCore::RTVal pathValue
      );  

    /// Casts the path the a humain readable format.
    /// Replaces the bindingID by the solverID if it exists.
    /// If not (only one binding), removes the bindingID from the path.
    void castPathToHRFormat(
      FabricCore::RTVal pathValue
      );

    /// Solver ID
    QString m_id;
    FabricCore::DFGBinding m_binding;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_PATH_VALUE_RESOLVER__
