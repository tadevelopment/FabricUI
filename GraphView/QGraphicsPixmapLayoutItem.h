// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_LockIcon__
#define __UI_GraphView_LockIcon__

#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsLayoutItem>

namespace FabricUI {
namespace GraphView {

class QGraphicsPixmapLayoutItem
  : public QGraphicsPixmapItem
  , public QGraphicsLayoutItem
{
public:

  QGraphicsPixmapLayoutItem(
    QPixmap const &pixmap
    )
    : QGraphicsPixmapItem( pixmap )
  {
    setGraphicsItem( this );
  }

  virtual QSizeF sizeHint(
    Qt::SizeHint which,
    const QSizeF & constraint = QSizeF()
    ) const
  {
    return pixmap().size();
  }

  virtual void setGeometry( const QRectF & rect )
  {
    setPos( rect.topLeft() );
  }
};

} // namespace GraphView
} // namespace FabricUI

#endif // __UI_GraphView_LockIcon__

