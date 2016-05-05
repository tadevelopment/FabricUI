//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef __UI_GraphView_InstBlock__
#define __UI_GraphView_InstBlock__

#include <QtGui/QGraphicsWidget>

namespace FabricUI {
namespace GraphView {

class InstBlockHeader;
class Node;

class InstBlock : public QGraphicsWidget
{
  Q_OBJECT

public:

  InstBlock(
    Node *node,
    QString name
    );

  Node *node()
    { return m_node; }

protected:

  virtual void paint(
    QPainter *painter,
    QStyleOptionGraphicsItem const *option,
    QWidget *widget
    ) /*override*/;

private:

  Node *m_node;
  InstBlockHeader *m_instBlockHeader;
};

} // namespace GraphView
} // namespace FabricUI

#endif // __UI_GraphView_InstBlock__
