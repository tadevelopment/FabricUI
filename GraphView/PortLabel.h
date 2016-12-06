//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/GraphView/TextContainer.h>

namespace FabricUI {
namespace GraphView {

class Port;

class PortLabel : public TextContainer
{
public:

  PortLabel(
    Port * parent,
    QString const &text,
    QColor color,
    QColor hlColor,
    QFont font
    );

protected:

  virtual void mousePressEvent( QGraphicsSceneMouseEvent * event );
  virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
  virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event );
  virtual void submitEditedText( const QString& text ); // override
  virtual void displayedTextChanged() FTL_OVERRIDE;

private:

  QPointF m_dragStartPosition;
  Port* m_port;
};

} // namespace GraphView
} // namespace FabricUI
