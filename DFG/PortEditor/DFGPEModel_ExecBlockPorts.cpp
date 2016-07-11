//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecBlockPorts.h>
#include <FTL/JSONValue.h>

namespace FabricUI {
namespace DFG {

DFGPEModel_ExecBlockPorts::DFGPEModel_ExecBlockPorts(
  DFGUICmdHandler *cmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  QSharedPointer<DFG::DFGExecNotifier> execNotifier,
  FTL::StrRef execBlockName
  )
  : DFGPEModel( "port", "Port" )
  , m_cmdHandler( cmdHandler )
  , m_binding( binding )
  , m_execPathQS( QString::fromUtf8( execPath.data(), execPath.size() ) )
  , m_exec( exec )
  , m_notifier( execNotifier )
  , m_execBlockName( execBlockName )
  , m_execBlockNameQS( QString::fromUtf8( execBlockName.data(), execBlockName.size() ) )
{
  connect(
    m_notifier.data(), SIGNAL(editWouldSplitFromPresetMayHaveChanged()),
    this, SLOT(onEditWouldSplitFromPresetMayHaveChanged())
    );
  connect(
    m_notifier.data(), SIGNAL(execBlockRenamed(unsigned, FTL::CStrRef, FTL::CStrRef)),
    this, SLOT(onExecBlockRenamed(unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(), SIGNAL(execBlockPortInserted(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::JSONObject const *)),
    this, SLOT(onExecBlockPortInserted(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::JSONObject const *))
    );
  connect(
    m_notifier.data(), SIGNAL(execBlockPortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef)),
    this, SLOT(onExecBlockPortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef))
    );

  init();
}

bool DFGPEModel_ExecBlockPorts::computeIsReadOnly()
{
  return m_exec.editWouldSplitFromPreset();
}

int DFGPEModel_ExecBlockPorts::getElementCount()
{
  return m_exec.getItemPortCount(
    m_execBlockName.c_str()
    );
}

QString DFGPEModel_ExecBlockPorts::getElementName( int index )
{
  FTL::CStrRef result = m_exec.getItemPortName(
    m_execBlockName.c_str(),
    index
    );
  return QString::fromUtf8( result.data(), result.size() );
}

FabricCore::DFGPortType DFGPEModel_ExecBlockPorts::getElementPortType( int index )
{
  return FabricCore::DFGPortType_In;
}

QString DFGPEModel_ExecBlockPorts::getElementTypeSpec( int index )
{
  FTL::CStrRef result = m_exec.getItemPortTypeSpec(
    m_execBlockName.c_str(),
    index
    );
  return QString::fromUtf8( result.data(), result.size() );
}

void DFGPEModel_ExecBlockPorts::insertElement(
  int index,
  QString desiredName,
  FabricCore::DFGPortType type,
  QString typeSpec
  )
{
  m_cmdHandler->dfgDoAddBlockPort(
    m_binding,
    m_execPathQS,
    m_exec,
    m_execBlockNameQS,
    desiredName,
    type,
    typeSpec,
    QString(), // pathToConnect
    FabricCore::DFGPortType_In, // connectType - ignored
    QString(), // extDep
    QString() // metadata
    );
}

void DFGPEModel_ExecBlockPorts::inspectElement(
  int index,
  DFGWidget *dfgWidget
  )
{
  assert( false );
}

void DFGPEModel_ExecBlockPorts::renameElement(
  int index,
  QString newName
  )
{
  QString oldPortPath = m_execBlockNameQS;
  oldPortPath += '.';
  oldPortPath += getElementName( index );

  m_cmdHandler->dfgDoEditPort(
    m_binding,
    m_execPathQS,
    m_exec,
    oldPortPath,
    newName,
    getElementPortType( index ),
    getElementTypeSpec( index ),
    QString(), // extDep
    QString() // uiMetadata
    );
}

void DFGPEModel_ExecBlockPorts::setElementPortType(
  int index,
  FabricCore::DFGPortType type
  )
{
  assert( false );
}

void DFGPEModel_ExecBlockPorts::setElementTypeSpec(
  int index,
  QString newTypeSpec
  )
{
  QString portName = getElementName( index );

  QString portPath = m_execBlockNameQS;
  portPath += '.';
  portPath += portName;

  m_cmdHandler->dfgDoEditPort(
    m_binding,
    m_execPathQS,
    m_exec,
    portPath,
    portName,
    getElementPortType( index ),
    newTypeSpec,
    QString(), // extDep
    QString() // uiMetadata
    );
}

void DFGPEModel_ExecBlockPorts::removeElements(
  QList<int> indices
  )
{
  qSort( indices );
  for ( int i = indices.size(); i--; )
  {
    int index = indices[i];

    QString portPath = m_execBlockNameQS;
    portPath += '.';
    portPath += getElementName( index );

    m_cmdHandler->dfgDoRemovePort(
      m_binding,
      m_execPathQS,
      m_exec,
      portPath
      );
  }
}

void DFGPEModel_ExecBlockPorts::reorderElements(
  QList<int> newIndices
  )
{
  assert( false );
}

void DFGPEModel_ExecBlockPorts::onEditWouldSplitFromPresetMayHaveChanged()
{
  updateIsReadOnly();
}

void DFGPEModel_ExecBlockPorts::onExecBlockRenamed(
  unsigned blockIndex,
  FTL::CStrRef oldExecBlockName,
  FTL::CStrRef newExecBlockName
  )
{
  if ( oldExecBlockName == m_execBlockName )
  {
    m_execBlockName = newExecBlockName;
    m_execBlockNameQS =
      QString::fromUtf8( newExecBlockName.data(), newExecBlockName.size() );
  }
}

void DFGPEModel_ExecBlockPorts::onExecBlockPortInserted(
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName,
  FTL::JSONObject const *portDesc
  )
{
  if ( blockName == m_execBlockName )
  {
    FTL::StrRef typeSpec = portDesc->getStringOrEmpty( FTL_STR("typeSpec") );
    emit elementInserted(
      portIndex,
      QString::fromUtf8( portName.data(), portName.size() ),
      FabricCore::DFGPortType_In,
      QString::fromUtf8( typeSpec.data(), typeSpec.size() )
      );
  }
}

void DFGPEModel_ExecBlockPorts::onExecBlockPortRemoved(
  FTL::CStrRef blockName,
  unsigned portIndex,
  FTL::CStrRef portName
  )
{
  if ( blockName == m_execBlockName )
    emit elementRemoved( portIndex );
}

bool DFGPEModel_ExecBlockPorts::isElementReadOnlyImpl( int index )
{
  return m_exec.isDepsExecBlockPort( m_execBlockName.c_str(), index );
}

} // namespace DFG
} // namespace FabricUI
