//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecPorts.h>
#include <FTL/JSONValue.h>

namespace FabricUI {
namespace DFG {

DFGPEModel_ExecPorts::DFGPEModel_ExecPorts(
  DFGUICmdHandler *cmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  QSharedPointer<DFG::DFGExecNotifier> execNotifier
  )
  : DFGPEModel( "port", "Port" )
  , m_cmdHandler( cmdHandler )
  , m_binding( binding )
  , m_execPathQS( QString::fromUtf8( execPath.data(), execPath.size() ) )
  , m_exec( exec )
  , m_notifier( execNotifier )
{
  connect(
    m_notifier.data(), SIGNAL(editWouldSplitFromPresetMayHaveChanged()),
    this, SLOT(onEditWouldSplitFromPresetMayHaveChanged())
    );
  connect(
    m_notifier.data(), SIGNAL(execPortInserted(unsigned, FTL::CStrRef, FTL::JSONObject const *)),
    this, SLOT(onExecPortInserted(unsigned, FTL::CStrRef, FTL::JSONObject const *))
    );
  connect(
    m_notifier.data(), SIGNAL(execPortRenamed(unsigned, FTL::CStrRef, FTL::CStrRef)),
    this, SLOT(onExecPortRenamed(unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(), SIGNAL(execPortTypeChanged(unsigned, FTL::CStrRef, FabricCore::DFGPortType)),
    this, SLOT(onExecPortTypeChanged(unsigned, FTL::CStrRef, FabricCore::DFGPortType))
    );
  connect(
    m_notifier.data(), SIGNAL(execPortTypeSpecChanged(unsigned, FTL::CStrRef, FTL::CStrRef)),
    this, SLOT(onExecPortTypeSpecChanged(unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(), SIGNAL(execPortRemoved(unsigned, FTL::CStrRef)),
    this, SLOT(onExecPortRemoved(unsigned, FTL::CStrRef))
    );
  connect(
    m_notifier.data(), SIGNAL(execPortsReordered(FTL::ArrayRef<unsigned>)),
    this, SLOT(onExecPortsReordered(FTL::ArrayRef<unsigned>))
    );

  init();
}

bool DFGPEModel_ExecPorts::computeIsReadOnly()
{
  return m_exec.editWouldSplitFromPreset();
}

int DFGPEModel_ExecPorts::getPortCount()
{
  return m_exec.getExecPortCount();
}

QString DFGPEModel_ExecPorts::getPortName( int index )
{
  FTL::CStrRef result = m_exec.getExecPortName( index );
  return QString::fromUtf8( result.data(), result.size() );
}

FabricCore::DFGPortType DFGPEModel_ExecPorts::getPortType( int index )
{
  return m_exec.getExecPortType( index );
}

QString DFGPEModel_ExecPorts::getPortTypeSpec( int index )
{
  FTL::CStrRef result = m_exec.getExecPortTypeSpec( index );
  return QString::fromUtf8( result.data(), result.size() );
}

bool DFGPEModel_ExecPorts::isPortReadOnlyImpl( int index )
{
  return m_exec.isDepsExecPort( index );
}

void DFGPEModel_ExecPorts::insertPort(
  int index,
  QString desiredPortName,
  FabricCore::DFGPortType portType,
  QString portTypeSpec
  )
{
  m_cmdHandler->dfgDoAddPort(
    m_binding,
    m_execPathQS,
    m_exec,
    desiredPortName,
    portType,
    portTypeSpec,
    QString(), // portToConnect
    QString(), // extDep
    QString() // metaData
    );
}

void DFGPEModel_ExecPorts::inspectPort(
  int index,
  DFGWidget *dfgWidget
  )
{
  dfgWidget->editExecPort( m_exec.getExecPortName( index ) );
}

void DFGPEModel_ExecPorts::renamePort(
  int index,
  QString newPortName
  )
{
  QString oldPortName = getPortName( index );

  m_cmdHandler->dfgDoEditPort(
    m_binding,
    m_execPathQS,
    m_exec,
    oldPortName,
    newPortName,
    getPortType( index ),
    getPortTypeSpec( index ),
    QString(), // extDep
    QString() // uiMetadata
    );
}

void DFGPEModel_ExecPorts::setPortType(
  int index,
  FabricCore::DFGPortType portType
  )
{
  QString portName = getPortName( index );

  m_cmdHandler->dfgDoEditPort(
    m_binding,
    m_execPathQS,
    m_exec,
    portName,
    portName,
    portType,
    getPortTypeSpec( index ),
    QString(), // extDep
    QString() // uiMetadata
    );
}

void DFGPEModel_ExecPorts::setPortTypeSpec(
  int index,
  QString newPortTypeSpec
  )
{
  QString portName = getPortName( index );

  m_cmdHandler->dfgDoEditPort(
    m_binding,
    m_execPathQS,
    m_exec,
    portName,
    portName,
    getPortType( index ),
    newPortTypeSpec,
    QString(), // extDep
    QString() // uiMetadata
    );
}

void DFGPEModel_ExecPorts::removePort(
  int index
  )
{
  m_cmdHandler->dfgDoRemovePort(
    m_binding,
    m_execPathQS,
    m_exec,
    getPortName( index )
    );
}

void DFGPEModel_ExecPorts::reorderPorts(
  QList<int> newIndices
  )
{
  m_cmdHandler->dfgDoReorderPorts(
    m_binding,
    m_execPathQS,
    m_exec,
    newIndices
    );
}

void DFGPEModel_ExecPorts::onEditWouldSplitFromPresetMayHaveChanged()
{
  updateIsReadOnly();
}

static FabricCore::DFGPortType PortTypeStrToDFGPortType( FTL::StrRef portTypeStr )
{
  if ( portTypeStr == FTL_STR("Out") )
    return FabricCore::DFGPortType_Out;
  else if ( portTypeStr == FTL_STR("IO") )
    return FabricCore::DFGPortType_IO;
  else
    return FabricCore::DFGPortType_In;
}

void DFGPEModel_ExecPorts::onExecPortInserted(
  unsigned portIndex,
  FTL::CStrRef portName,
  FTL::JSONObject const *portDesc
  )
{
  FTL::StrRef typeSpec = portDesc->getStringOrEmpty( FTL_STR("typeSpec") );
  emit portInserted(
    portIndex,
    QString::fromUtf8( portName.data(), portName.size() ),
    PortTypeStrToDFGPortType( portDesc->getStringOrEmpty( FTL_STR("outsidePortType") ) ),
    QString::fromUtf8( typeSpec.data(), typeSpec.size() )
    );
}

void DFGPEModel_ExecPorts::onExecPortRenamed(
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName
  )
{
  emit portRenamed(
    portIndex,
    QString::fromUtf8( newPortName.data(), newPortName.size() )
    );
}

void DFGPEModel_ExecPorts::onExecPortTypeChanged(
  unsigned portIndex,
  FTL::CStrRef portName,
  FabricCore::DFGPortType newPortType
  )
{
  emit portTypeChanged( portIndex, newPortType );
}

void DFGPEModel_ExecPorts::onExecPortTypeSpecChanged(
  unsigned portIndex,
  FTL::CStrRef portName,
  FTL::CStrRef newPortTypeSpec
  )
{
  emit portTypeSpecChanged(
    portIndex,
    QString::fromUtf8( newPortTypeSpec.data(), newPortTypeSpec.size() )
    );
}

void DFGPEModel_ExecPorts::onExecPortRemoved(
  unsigned portIndex,
  FTL::CStrRef portName
  )
{
  emit portRemoved( portIndex );
}

void DFGPEModel_ExecPorts::onExecPortsReordered(
  FTL::ArrayRef<unsigned> newOrder
  )
{
  QList<int> newIndices;
  for ( size_t i = 0; i < newOrder.size(); ++i )
    newIndices.push_back( newOrder[i] );
  emit portsReordered( newIndices );
}

} // namespace DFG
} // namespace FabricUI
