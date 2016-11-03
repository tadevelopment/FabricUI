//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef __UI_DFG_DFGBINDINGUTILS__
#define __UI_DFG_DFGBINDINGUTILS__

#include <QList>
#include <QString>
#include <QStringList>
#include <FTL/StrRef.h>
#include <FabricCore.h>

namespace FabricUI {
namespace DFG {

class DFGBindingUtils 
{
  public:
    DFGBindingUtils();

    /// Gets the names of all the variables in the graph
    /// whose type is present in the varTypes list.
    static QStringList getVariableWordsFromBinding(
      FabricCore::DFGBinding &binding,
      FTL::CStrRef currentExecPath,
      QStringList varTypes);      

    /// Gets the values of all the variables in the graph
    /// whose type is present in the varTypes list.
    static QList<FabricCore::RTVal> getVariableValuesFromBinding(
      FabricCore::DFGBinding &binding,
      FTL::CStrRef currentExecPath,
      QStringList varTypes);

    /// Gets the names of all the variables in the graph.
    static QStringList getVariableWordsFromBinding(
      FabricCore::DFGBinding &binding, 
      FTL::CStrRef currentExecPath);

    /// Gets the preset path for this node's name (if it's a preset)
    static QString getPresetPathFromNode(
      FabricCore::DFGExec &exec, 
      QString nodeName);
};

} // namespace FabricUI
} // namespace DFG

#endif // __UI_DFG_DFGBINDINGUTILS__
