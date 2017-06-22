//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H
#define FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H

#include <QGraphicsView>
#include <FTL/Config.h>

namespace FabricUI
{
namespace FCurveEditor
{

class RuledGraphicsView : public QGraphicsView
{
  Q_OBJECT

public:
  RuledGraphicsView();

protected:
  void wheelEvent( QWheelEvent * ) FTL_OVERRIDE;
  void drawBackground( QPainter *, const QRectF & ) FTL_OVERRIDE;

private:
  float m_scrollSpeed = 1 / 800.0f;
};

} // namespace FCurveEditor
} // namespace ValueEditor 

#endif // FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H
