//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef _DFGPEModel_ExecBlockPorts_h
#define _DFGPEModel_ExecBlockPorts_h

#include <FTL/StrRef.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel.h>
#include <FabricUI/DFG/DFGExecNotifier.h>

namespace FabricUI {
namespace DFG {

class DFGUICmdHandler;

class DFGPEModel_ExecBlockPorts : public DFGPEModel
{
  Q_OBJECT
  
public:

  DFGPEModel_ExecBlockPorts(
    DFGUICmdHandler *cmdHandler,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    QSharedPointer<DFG::DFGExecNotifier> execNotifier,
    FTL::StrRef execBlockName
    );

  virtual bool canInspectElements() /*override*/ { return false; }
  virtual bool hasPortType() /*override*/ { return false; }
  virtual bool hasTypeSpec() /*override*/ { return true; }

  virtual int getElementCount() /*override*/;
  virtual QString getElementName( int index ) /*override*/;
  virtual FabricCore::DFGPortType getElementPortType( int index ) /*override*/;
  virtual QString getElementTypeSpec( int index ) /*override*/;

  virtual void insertElement(
    int index,
    QString desiredName,
    FabricCore::DFGPortType type,
    QString typeSpec
    ) /*override*/;
  virtual void inspectElement(
    int index,
    DFGWidget *dfgWidget
    ) /*override*/;
  virtual void renameElement(
    int index,
    QString newName
    ) /*override*/;
  virtual void setElementPortType(
    int index,
    FabricCore::DFGPortType type
    ) /*override*/;
  virtual void setElementTypeSpec(
    int index,
    QString newTypeSpec
    ) /*override*/;
  virtual void removeElement(
    int index
    ) /*override*/;

  virtual void reorderElements(
    QList<int> newIndices
    ) /*override*/;

protected:

  virtual bool isElementReadOnlyImpl( int index ) /*override*/;
  virtual bool computeIsReadOnly() /*override*/;

protected slots:

  void onEditWouldSplitFromPresetMayHaveChanged();

  void onExecBlockRenamed(
    unsigned blockIndex,
    FTL::CStrRef oldExecBlockName,
    FTL::CStrRef newExecBlockName
    );

  void onExecBlockPortInserted(
    FTL::CStrRef blockName,
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::JSONObject const *portDesc
    );

  void onExecBlockPortRemoved(
    FTL::CStrRef blockName,
    unsigned portIndex,
    FTL::CStrRef portName
    );

private:

  DFGUICmdHandler *m_cmdHandler;
  FabricCore::DFGBinding m_binding;
  QString m_execPathQS;
  FabricCore::DFGExec m_exec;
  QSharedPointer<DFG::DFGExecNotifier> m_notifier;
  std::string m_execBlockName;
  QString m_execBlockNameQS;
};

} // namespace DFG
} // namespace FabricUI

#endif // _DFGPEModel_ExecBlockPorts_h
