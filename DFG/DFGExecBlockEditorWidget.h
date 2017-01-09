// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGExecBlockEditorWidget__
#define __UI_DFG_DFGExecBlockEditorWidget__

#include <QFrame>

class QVBoxLayout;

namespace FabricUI {
namespace DFG {

class DFGExecHeaderWidget;
class DFGPEWidget_Elements;
class DFGWidget;

class DFGExecBlockEditorWidget : public QFrame
{
  Q_OBJECT

public:

  DFGExecBlockEditorWidget(
    DFGWidget *dfgWidget,
    DFGExecHeaderWidget *dfgExecHeaderWidget
    );
  virtual ~DFGExecBlockEditorWidget();

public slots:

  void onExecChanged();

private:

  DFGWidget *m_dfgWidget;
  QVBoxLayout *m_layout;
  DFGPEWidget_Elements *m_peElementsWidget;
};

} // namespace DFG
} // namespace FabricUI

#endif // __UI_DFG_DFGExecBlockEditorWidget__
