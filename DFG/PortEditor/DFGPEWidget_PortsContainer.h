//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef _DFGPEWidget_PortsContainer_h
#define _DFGPEWidget_PortsContainer_h

#include <QtGui/QFrame>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

namespace FabricUI {
namespace DFG {

class DFGPEModel;
class DFGPEWidget_Ports;
class DFGWidget;

class DFGPEWidget_PortsContainer : public QFrame
{
  Q_OBJECT

public:

  DFGPEWidget_PortsContainer(
    DFGWidget *dfgWidget,
    QString desc,
    DFGPEModel *model,
    QWidget *parent = NULL
    );

private:

  DFGPEWidget_Ports *m_ports;
};

} // namespace DFG
} // namespace FabricUI

#endif // _DFGPEWidget_PortsContainer_h
