//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef _DFGPEWidget_Exec_h
#define _DFGPEWidget_Exec_h

#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <QtGui/QFrame>

class QTabWidget;
class QVBoxLayout;

namespace FabricUI {
namespace DFG {

class DFGPEWidget_PortsContainer;
class DFGWidget;

class DFGPEWidget_Exec : public QFrame
{
  Q_OBJECT

public:

  DFGPEWidget_Exec(
    DFGWidget *dfgWidget,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    QWidget *parent = NULL
    );

  void setExec(
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec
    );

private:

  DFGWidget *m_dfgWidget;
  QTabWidget *m_tabWidget;
  QVBoxLayout *m_layout;
};

} // namespace DFG
} // namespace FabricUI

#endif // _DFGPEWidget_Exec_h
