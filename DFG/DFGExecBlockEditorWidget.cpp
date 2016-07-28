// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFontMetrics>
#include <QSplitter>
#include <QMessageBox>

#include <FabricUI/DFG/PortEditor/DFGPEWidget_Elements.h>
#include <FabricUI/DFG/PortEditor/DFGPEModel_ExecBlockPorts.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
#include <FabricUI/DFG/DFGExecBlockEditorWidget.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGExecHeaderWidget.h>

namespace FabricUI {
namespace DFG {

DFGExecBlockEditorWidget::DFGExecBlockEditorWidget(
  DFGWidget *dfgWidget,
  DFGExecHeaderWidget *dfgExecHeaderWidget
  )
  : QFrame( dfgWidget )
  , m_dfgWidget( dfgWidget )
  , m_peElementsWidget( NULL )
{
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  m_layout = new QVBoxLayout;
  m_layout->setContentsMargins( 0, 0, 0, 0 );
  setLayout( m_layout );

  QObject::connect(
    m_dfgWidget->getDFGController(), SIGNAL(execChanged()),
    this, SLOT(onExecChanged())
    );
}

DFGExecBlockEditorWidget::~DFGExecBlockEditorWidget()
{
}

void DFGExecBlockEditorWidget::onExecChanged()
{
  if ( m_peElementsWidget )
  {
    m_layout->removeWidget( m_peElementsWidget );
    m_peElementsWidget = NULL;
  }

  DFGController *dfgController = m_dfgWidget->getDFGController();
  FabricCore::DFGExec exec = dfgController->getExec();
  FTL::StrRef execBlockName = dfgController->getExecBlockName();
  if ( !exec.isValid() || execBlockName.empty() )
  {
    hide();
    return;
  }

  m_peElementsWidget =
    new DFGPEWidget_Elements(
      m_dfgWidget,
      new DFGPEModel_ExecBlockPorts(
        dfgController->getCmdHandler(),
        dfgController->getBinding(),
        dfgController->getExecPath(),
        exec,
        DFGExecNotifier::Create( exec ),
        execBlockName
        )
      );
  m_layout->addWidget( m_peElementsWidget );

  show();
}

} // namespace DFG
} // namespace FabricUI
