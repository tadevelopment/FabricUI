//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecPorts.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecBlockPorts.h>
#include <FabricUI/DFG/PortEditor/DFGPEWidget_Exec.h>
#include <FabricUI/DFG/PortEditor/DFGPEWidget_Elements.h>
#include <FabricUI/Util/LoadPixmap.h>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>

namespace FabricUI {
namespace DFG {

DFGPEWidget_Exec::DFGPEWidget_Exec(
  DFGWidget *dfgWidget,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec,
  QWidget *parent
  )
  : QFrame( parent )
  , m_dfgWidget( dfgWidget )
  , m_plusIcon( QIcon( LoadPixmap( "DFGPlus.png" ) ) )
{
  setObjectName( "DFGPEWidget_Exec" );

  m_tabWidget = new QTabWidget;

  m_layout = new QVBoxLayout;
  m_layout->setContentsMargins( 0, 0, 0, 0 );
  m_layout->addWidget( m_tabWidget );
  setLayout( m_layout );

  setExec( binding, execPath, exec );
}

DFGPEWidget_Exec::~DFGPEWidget_Exec()
{
}

void DFGPEWidget_Exec::setExec(
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec
  )
{
  m_tabWidget->clear();

  if ( !exec.isValid() )
    return;
  m_binding = binding;
  m_execPath = execPath;
  m_execPathQS = QString::fromUtf8( execPath.data(), execPath.size() );
  m_exec = exec;
  m_execNotifier = DFGExecNotifier::Create( exec );

  connect(
    m_execNotifier.data(), SIGNAL(execBlockInserted(unsigned, FTL::CStrRef)),
    this, SLOT(onExecBlockInserted(unsigned, FTL::CStrRef))
    );
  connect(
    m_execNotifier.data(), SIGNAL(execBlockRemoved(unsigned, FTL::CStrRef)),
    this, SLOT(onExecBlockRemoved(unsigned, FTL::CStrRef))
    );

  DFGPEModel *execPortsModel =
    new DFGPEModel_ExecPorts(
      m_dfgWidget->getDFGController()->getCmdHandler(),
      binding,
      execPath,
      exec,
      m_execNotifier
      );

  DFGPEWidget_Elements *funcPorts =
    new DFGPEWidget_Elements(
      m_dfgWidget,
      execPortsModel
      );

  m_tabWidget->addTab( funcPorts, "Function Ports" );

  unsigned execBlockCount = exec.getExecBlockCount();
  for ( unsigned execBlockIndex = 0;
    execBlockIndex < execBlockCount; ++execBlockIndex )
  {
    FTL::CStrRef execBlockName = exec.getExecBlockName( execBlockIndex );
    onExecBlockInserted( execBlockIndex, execBlockName );
  }

  m_addBlockLineEdit = new QLineEdit;
  m_addBlockButton = new QPushButton( m_plusIcon, "Add block" );
  connect(
    m_addBlockButton, SIGNAL(clicked()),
    this, SLOT(onAddBlockButtonClicked())
    );
  QHBoxLayout *addBlockLayout = new QHBoxLayout;
  addBlockLayout->setContentsMargins( 0, 0, 0, 0 );
  addBlockLayout->addWidget( new QLabel("Block name:") );
  addBlockLayout->addWidget( m_addBlockLineEdit );
  addBlockLayout->addWidget( m_addBlockButton );
  QFrame *addBlockContainer = new QFrame;
  addBlockContainer->setObjectName( "DFGAddBlockContainer" );
  addBlockContainer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  addBlockContainer->setLayout( addBlockLayout );
  m_tabWidget->addTab( addBlockContainer, "Add Block" );
}

void DFGPEWidget_Exec::onAddBlockButtonClicked()
{
  int execBlockIndex = m_exec.getExecBlockCount();
  DFGUICmdHandler *cmdHandler =
    m_dfgWidget->getDFGController()->getCmdHandler();
  cmdHandler->dfgDoAddBlock(
    m_binding,
    m_execPathQS,
    m_exec,
    m_addBlockLineEdit->text(),
    QPointF( 0, 0 )
    );
  m_tabWidget->setCurrentIndex( 1 + execBlockIndex );
}

void DFGPEWidget_Exec::onExecBlockInserted(
  unsigned blockIndex,
  FTL::CStrRef blockName
  )
{
  QString desc = "Block '";
  desc += QString::fromUtf8( blockName.data(), blockName.size() );
  desc += '\'';

  DFGPEModel *execBlockPortsModel =
    new DFGPEModel_ExecBlockPorts(
      m_dfgWidget->getDFGController()->getCmdHandler(),
      m_binding,
      m_execPath,
      m_exec,
      m_execNotifier,
      blockName
      );

  DFGPEWidget_Elements *execBlockPorts =
    new DFGPEWidget_Elements(
      m_dfgWidget,
      execBlockPortsModel
      );

  m_tabWidget->insertTab( 1 + blockIndex, execBlockPorts, desc );
}

void DFGPEWidget_Exec::onExecBlockRemoved(
  unsigned blockIndex,
  FTL::CStrRef blockName
  )
{
  m_tabWidget->removeTab( blockIndex );
}

} // namespace DFG
} // namespace FabricUI
