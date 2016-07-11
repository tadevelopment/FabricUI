//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecBlocks.h>
#include <FTL/JSONValue.h>

namespace FabricUI {
namespace DFG {

DFGPEModel_ExecBlocks::DFGPEModel_ExecBlocks(
  DFGUICmdHandler *cmdHandler,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  QSharedPointer<DFG::DFGExecNotifier> execNotifier
  )
  : DFGPEModel( "block", "Block" )
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
    m_notifier.data(), SIGNAL(execBlockInserted(unsigned, FTL::CStrRef)),
    this, SLOT(onExecBlockInserted(unsigned, FTL::CStrRef))
    );
  connect(
    m_notifier.data(), SIGNAL(execBlockRenamed(unsigned, FTL::CStrRef, FTL::CStrRef)),
    this, SLOT(onExecBlockRenamed(unsigned, FTL::CStrRef, FTL::CStrRef))
    );
  connect(
    m_notifier.data(), SIGNAL(execBlockRemoved(unsigned, FTL::CStrRef)),
    this, SLOT(onExecBlockRemoved(unsigned, FTL::CStrRef))
    );
  // connect(gi

  init();
}

bool DFGPEModel_ExecBlocks::computeIsReadOnly()
{
  return m_exec.editWouldSplitFromPreset();
}

int DFGPEModel_ExecBlocks::getElementCount()
{
  return m_exec.getExecBlockCount();
}

QString DFGPEModel_ExecBlocks::getElementName( int index )
{
  FTL::CStrRef result = m_exec.getExecBlockName( index );
  return QString::fromUtf8( result.data(), result.size() );
}

FabricCore::DFGPortType DFGPEModel_ExecBlocks::getElementPortType( int index )
{
  return FabricCore::DFGPortType_In;
}

QString DFGPEModel_ExecBlocks::getElementTypeSpec( int index )
{
  return QString();
}

bool DFGPEModel_ExecBlocks::isElementReadOnlyImpl( int index )
{
  return false;
}

void DFGPEModel_ExecBlocks::insertElement(
  int index,
  QString desiredName,
  FabricCore::DFGPortType portType,
  QString typeSpec
  )
{
  m_cmdHandler->dfgDoAddBlock(
    m_binding,
    m_execPathQS,
    m_exec,
    desiredName,
    QPointF( 0, 0 )
    );
}

void DFGPEModel_ExecBlocks::inspectElement(
  int index,
  DFGWidget *dfgWidget
  )
{
  assert( false );
}

void DFGPEModel_ExecBlocks::renameElement(
  int index,
  QString newPortName
  )
{
  m_cmdHandler->dfgDoEditNode(
    m_binding,
    m_execPathQS,
    m_exec,
    getElementName( index ),
    newPortName,
    QString(), // nodeMetadata
    QString()
    );
}

void DFGPEModel_ExecBlocks::setElementPortType(
  int index,
  FabricCore::DFGPortType portType
  )
{
  assert( false );
}

void DFGPEModel_ExecBlocks::setElementTypeSpec(
  int index,
  QString newPortTypeSpec
  )
{
  assert( false );
}

void DFGPEModel_ExecBlocks::removeElement(
  int index
  )
{
  QStringList execBlockNames;
  execBlockNames << getElementName( index );
  m_cmdHandler->dfgDoRemoveNodes(
    m_binding,
    m_execPathQS,
    m_exec,
    execBlockNames
    );
}

void DFGPEModel_ExecBlocks::reorderElements(
  QList<int> newIndices
  )
{
  assert( false );
}

void DFGPEModel_ExecBlocks::onEditWouldSplitFromPresetMayHaveChanged()
{
  updateIsReadOnly();
}

void DFGPEModel_ExecBlocks::onExecBlockInserted(
  unsigned blockIndex,
  FTL::CStrRef blockName
  )
{
  emit elementInserted(
    blockIndex,
    QString::fromUtf8( blockName.data(), blockName.size() ),
    FabricCore::DFGPortType_In,
    QString()
    );
}

void DFGPEModel_ExecBlocks::onExecBlockRenamed(
  unsigned blockIndex,
  FTL::CStrRef oldBlockName,
  FTL::CStrRef newBlockName
  )
{
  emit elementRenamed(
    blockIndex,
    QString::fromUtf8( newBlockName.data(), newBlockName.size() )
    );
}

void DFGPEModel_ExecBlocks::onExecBlockRemoved(
  unsigned blockIndex,
  FTL::CStrRef portName
  )
{
  emit elementRemoved( blockIndex );
}

void DFGPEModel_ExecBlocks::onExecBlocksReordered(
  FTL::ArrayRef<unsigned> newOrder
  )
{
  QList<int> newIndices;
  for ( size_t i = 0; i < newOrder.size(); ++i )
    newIndices.push_back( newOrder[i] );
  emit elementsReordered( newIndices );
}

} // namespace DFG
} // namespace FabricUI
