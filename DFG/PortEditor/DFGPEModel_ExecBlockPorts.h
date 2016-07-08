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
    FTL::StrRef execBlockName
    );

  virtual bool isPortTypeFixed() /*override*/ { return true; }

  virtual int getPortCount() /*override*/;
  virtual QString getPortName( int index ) /*override*/;
  virtual FabricCore::DFGPortType getPortType( int index ) /*override*/;
  virtual QString getPortTypeSpec( int index ) /*override*/;

  virtual void insertPort(
    int index,
    QString desiredName,
    FabricCore::DFGPortType type,
    QString typeSpec
    ) /*override*/;
  virtual void inspectPort(
    int index,
    DFGWidget *dfgWidget
    ) /*override*/;
  virtual void renamePort(
    int index,
    QString newName
    ) /*override*/;
  virtual void setPortType(
    int index,
    FabricCore::DFGPortType type
    ) /*override*/;
  virtual void setPortTypeSpec(
    int index,
    QString newTypeSpec
    ) /*override*/;
  virtual void removePort(
    int index
    ) /*override*/;

  virtual void reorderPorts(
    QList<int> newIndices
    ) /*override*/;

protected:

  virtual bool isPortReadOnlyImpl( int index ) /*override*/;
  virtual bool computeIsReadOnly() /*override*/;

protected slots:

  void onEditWouldSplitFromPresetMayHaveChanged();

  void onExecBlockRenamed(
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
  std::string m_execBlockName;
  QString m_execBlockNameQS;
  QSharedPointer<DFG::DFGExecNotifier> m_notifier;
};

} // namespace DFG
} // namespace FabricUI

#endif // _DFGPEModel_ExecBlockPorts_h
