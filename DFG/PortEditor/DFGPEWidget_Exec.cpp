//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecPorts.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecBlockPorts.h>
#include <FabricUI/DFG/PortEditor/DFGPEWidget_Exec.h>
#include <FabricUI/DFG/PortEditor/DFGPEWidget_Ports.h>
#include <QtGui/QLabel>
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
{
  setObjectName( "DFGPEWidget_Exec" );

  m_tabWidget = new QTabWidget;

  m_layout = new QVBoxLayout;
  m_layout->setContentsMargins( 0, 0, 0, 0 );
  m_layout->addWidget( m_tabWidget );
  setLayout( m_layout );

  setExec( binding, execPath, exec );
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

  DFGPEModel *execPortsModel =
    new DFGPEModel_ExecPorts(
      m_dfgWidget->getDFGController()->getCmdHandler(),
      binding,
      execPath,
      exec
      );

  DFGPEWidget_Ports *funcPorts =
    new DFGPEWidget_Ports(
      m_dfgWidget,
      execPortsModel
      );

  m_tabWidget->addTab( funcPorts, "Function Ports" );

  unsigned execBlockCount = exec.getExecBlockCount();
  for ( unsigned execBlockIndex = 0;
    execBlockIndex < execBlockCount; ++execBlockIndex )
  {
    FTL::CStrRef execBlockName = exec.getExecBlockName( execBlockIndex );

    QString desc = "Block '";
    desc += QString::fromUtf8( execBlockName.data(), execBlockName.size() );
    desc += '\'';

    DFGPEModel *execBlockPortsModel =
      new DFGPEModel_ExecBlockPorts(
        m_dfgWidget->getDFGController()->getCmdHandler(),
        binding,
        execPath,
        exec,
        execBlockName
        );

    DFGPEWidget_Ports *execBlockPorts =
      new DFGPEWidget_Ports(
        m_dfgWidget,
        execBlockPortsModel
        );

    m_tabWidget->addTab( execBlockPorts, desc );
  }
}

} // namespace DFG
} // namespace FabricUI
