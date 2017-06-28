//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H
#define FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H

#include <QWidget>
#include <FTL/Config.h>

class QTimer;
class QGraphicsView;

namespace FabricUI
{
namespace FCurveEditor
{

class RuledGraphicsView : public QWidget
{
  Q_OBJECT

public:
  RuledGraphicsView();
  QGraphicsView* view();

  // TODO : custom scale (see FCurveEditor::Ruler's properties)
  // TODO : fixed-point under the cursor when zooming (currently on the middle instead)
  // TODO : fix the "smoothZoom" on Linux

  void fitInView( const QRectF );

protected:
  void wheelEvent( QWheelEvent * ) FTL_OVERRIDE;
  void resizeEvent( QResizeEvent * ) FTL_OVERRIDE;

private slots:
  void tick();

private:

  void wheelEvent( int xDelta, int yDelta );

  class GraphicsView;
  GraphicsView* m_view;
  class Ruler;
  void updateRulersRange();
  Ruler* m_hRuler;
  Ruler* m_vRuler;

  float m_scrollSpeed;

  // Smooth zoom (animated)
  bool m_smoothZoom;
  QPointF m_targetScale;
  QTimer* m_timer;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H
