// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_BlockRectangle__
#define __UI_GraphView_BlockRectangle__

#include <QtWidgets/QGraphicsWidget>

namespace FabricUI {
namespace GraphView {

class Node;

class BlockRectangle : public QGraphicsWidget
{
public:

  BlockRectangle( Node *node );

  virtual void paint(
    QPainter *painter, 
    QStyleOptionGraphicsItem const *option,
    QWidget *widget
    ) /*override*/;

private:

  Node *m_node;
};


} // namespace GraphView
} // namespace FabricUI

#endif // __UI_GraphView_BlockRectangle__
