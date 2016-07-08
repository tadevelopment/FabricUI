//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/PortEditor/DFGPEWidget_Ports.h>
#include <FabricUI/DFG/PortEditor/DFGPEWidget_PortsContainer.h>
#include <FabricUI/Util/LoadPixmap.h>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

namespace FabricUI {
namespace DFG {

DFGPEWidget_PortsContainer::DFGPEWidget_PortsContainer(
  DFGWidget *dfgWidget,
  QString desc,
  DFGPEModel *model,
  QWidget *parent
  )
  : QFrame( parent )
{
  setObjectName( "DFGPEWidget_PortsContainer" );

  QLabel *descLabel = new QLabel( desc );
  descLabel->setObjectName( "DFGPEWidget_PortsContainer_Title" );
  descLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

  m_ports = new DFGPEWidget_Ports( dfgWidget, model );
  
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( descLabel );
  layout->addWidget( m_ports );

  setLayout( layout );
}

} // namespace DFG
} // namespace FabricUI
