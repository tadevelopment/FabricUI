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

  Q_PROPERTY( size_t rulersSize READ rulersSize WRITE setRulersSize )
  size_t m_rulersSize;
  Q_PROPERTY( bool topToBottomY READ topToBottomY() WRITE setTopToBottomY() )

  bool m_rectangleSelectionEnabled;


public:
  RuledGraphicsView();
  QGraphicsView* view();

  // TODO : custom scale (see FCurveEditor::Ruler's properties)
  // TODO : fix the "smoothZoom" on Linux

  void fitInView( const QRectF );
  inline size_t rulersSize() const { return m_rulersSize; }
  void setRulersSize( const size_t );
  bool topToBottomY() const;
  void setTopToBottomY( bool );
  inline void enableRectangleSelection( bool e ) { m_rectangleSelectionEnabled = e; }

signals:
  void rectangleSelectReleased( const QRectF& ) const;

protected:
  void wheelEvent( QWheelEvent * ) FTL_OVERRIDE;
  void resizeEvent( QResizeEvent * ) FTL_OVERRIDE;

private slots:
  void tick();

private:

  void wheelEvent(
    int xDelta,
    int yDelta,
    // center (in scene-space) of the scaling
    // it will be a fixed-point in the view coordinates
    QPointF scalingCenter
  );
  void centeredScale( qreal x, qreal y );

  class GraphicsView;
  GraphicsView* m_view;
  class Ruler;
  void updateRulersRange();
  Ruler* m_hRuler;
  Ruler* m_vRuler;

  float m_scrollSpeed;
  bool m_zoomOnCursor;

  // Smooth zoom (animated)
  bool m_smoothZoom;
  QPointF m_scalingCenter;
  QPointF m_targetScale;
  QTimer* m_timer;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RULEDGRAPHICSVIEW_H
