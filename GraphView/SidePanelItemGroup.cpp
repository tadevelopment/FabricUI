// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/SidePanelItemGroup.h>

using namespace FabricUI::GraphView;

SidePanelItemGroup::SidePanelItemGroup(QGraphicsWidget * parent)
: QGraphicsWidget(parent)
{
}

void SidePanelItemGroup::resizeEvent(QGraphicsSceneResizeEvent * event)
{
  QGraphicsWidget::resizeEvent(event);
  emit resized();
}
