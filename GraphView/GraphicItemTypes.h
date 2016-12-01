// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphicItemTypes__
#define __UI_GraphView_GraphicItemTypes__

#include <QGraphicsItem>

namespace FabricUI
{

  namespace GraphView
  {

    enum GraphicsItemTypes
    {
      QGraphicsItemType_Node = QGraphicsItem::UserType + 5,
      QGraphicsItemType_Pin,
      QGraphicsItemType_Port,
      QGraphicsItemType_FixedPort,
      QGraphicsItemType_ProxyPort,
      QGraphicsItemType_Connection,
      QGraphicsItemType_PinCircle,
      QGraphicsItemType_NodeRectangle,
      QGraphicsItemType_ProxyPin,
      QGraphicsItemType_NodeHeaderButton,
      QGraphicsItemType_InstBlock,
      QGraphicsItemType_InstBlockPort
    };

  };

};

#endif // __UI_GraphView_GraphicItemTypes__
