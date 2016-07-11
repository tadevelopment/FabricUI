//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef _DFGPEModel_ExecPorts_h
#define _DFGPEModel_ExecPorts_h

#include <FTL/StrRef.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel.h>
#include <FabricUI/DFG/DFGExecNotifier.h>

namespace FabricUI {
namespace DFG {

class DFGUICmdHandler;

class DFGPEModel_ExecPorts : public DFGPEModel
{
  Q_OBJECT

public:

  DFGPEModel_ExecPorts(
    DFGUICmdHandler *cmdHandler,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    QSharedPointer<DFG::DFGExecNotifier> execNotifier
    );

  virtual bool isPortTypeFixed() /*override*/ { return false; }

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

  void onExecPortInserted(
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::JSONObject const *portDesc
    );

  void onExecPortRenamed(
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void onExecPortTypeChanged(
    unsigned portIndex,
    FTL::CStrRef portName,
    FabricCore::DFGPortType newPortType
    );

  void onExecPortTypeSpecChanged(
    unsigned portIndex,
    FTL::CStrRef portName,
    FTL::CStrRef newTypeSpec
    );

  void onExecPortRemoved(
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void onExecPortsReordered(
    FTL::ArrayRef<unsigned> newOrder
    );

private:

  DFGUICmdHandler *m_cmdHandler;
  FabricCore::DFGBinding m_binding;
  QString m_execPathQS;
  FabricCore::DFGExec m_exec;
  QSharedPointer<DFG::DFGExecNotifier> m_notifier;
};

} // namespace DFG
} // namespace FabricUI

#endif // _DFGPEModel_ExecPorts_h
