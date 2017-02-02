// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DFGCollapsableHeaderWidget.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGCollapsableHeaderWidget::DFGCollapsableHeaderWidget(QWidget * parent, QString title, const DFGConfig & dfgConfig)
: QLabel(title, parent)
, m_dfgConfig(dfgConfig)
{
  setObjectName( "DFGCollapsableHeaderWidget" );
}

DFGCollapsableHeaderWidget::~DFGCollapsableHeaderWidget()
{
}

void DFGCollapsableHeaderWidget::mousePressEvent(QMouseEvent * event)
{
  emit toggled();
  event->accept();
}
