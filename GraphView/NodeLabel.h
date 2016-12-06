// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeLabel__
#define __UI_GraphView_NodeLabel__

#include "TextContainer.h"
#include <FTL/Config.h>

namespace FabricUI
{

  namespace GraphView
  {
    class Node;

    class NodeLabel : public TextContainer
    {
      Node *m_node;

    public:

      NodeLabel(
        QGraphicsWidget * parent,
        Node* node,
        QString const &text,
        QColor color,
        QColor highlightColor,
        QFont font
        );

    protected:
      // The label will grab the focus, but it will forward
      // unused events to the Node
      virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
      virtual void displayedTextChanged() FTL_OVERRIDE;
    };

  };

};

#endif // __UI_GraphView_NodeLabel__
