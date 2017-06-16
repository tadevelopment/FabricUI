// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphViewWidget__
#define __UI_GraphView_GraphViewWidget__

#include <QUrl>
#include <QGraphicsView>
#include <QColor>
#include <QPen>

#include "GraphConfig.h"

namespace FabricUI
{

  namespace GraphView
  {

    class Graph;

    // The GraphViewScene specializes QGraphicsScene for intercepting events that need to be treated
    // globally (eg: click for panning the camera) and not forwareded to individual scene widgets.
    // Note: using event filters on scene widgets doesn't work because 'hover' and 'mouse grabber'
    //       features are bypassing the normal event propagation process.
    class GraphViewScene : public QGraphicsScene
    {
      Q_OBJECT

    public:

      GraphViewScene(
        Graph * graph
        );

    signals:

      void urlDropped( QUrl url, bool ctrlPressed, bool altPressed, QPointF pos  );

    protected:

      virtual void dragEnterEvent( QGraphicsSceneDragDropEvent *event );
      virtual void dropEvent( QGraphicsSceneDragDropEvent *event );

    private:

      Graph * m_graph;
    };

    class GraphViewWidget : public QGraphicsView
    {
      Q_OBJECT

    public:

      GraphViewWidget(
        QWidget * parent,
        const GraphConfig & config = GraphConfig(),
        Graph * graph = NULL
        );

      Graph * graph();
      const Graph * graph() const;
      virtual void setGraph(Graph * graph);

      virtual void resizeEvent(QResizeEvent * event);
      virtual void mousePressEvent(QMouseEvent * event);
      virtual void mouseMoveEvent(QMouseEvent * event);
      virtual void keyPressEvent(QKeyEvent * event);
      virtual void contextMenuEvent(QContextMenuEvent * event);
      QPoint lastEventPos() const;

      QPointF mapToGraph( QPoint const &globalPos ) const;
      
      float getUiGraphZoomBeforeQuickZoom()
        { return m_uiGraphZoomBeforeQuickZoom; }
      void setUiGraphZoomBeforeQuickZoom(float zoom)
        { m_uiGraphZoomBeforeQuickZoom = zoom; }
      
    public slots:

      void onSceneChanged();

    signals:

      void sceneChanged();
      void urlDropped( QUrl url, bool ctrlPressed, bool altPressed, QPointF pos );
 
    protected:

      virtual bool focusNextPrevChild(bool next);
      
      virtual void drawBackground(QPainter *painter, const QRectF &exposedRect);

    private:

      QPoint m_lastEventPos;
      GraphViewScene * m_scene;
      Graph * m_graph;
      bool m_altWasHeldAtLastMousePress;
      std::vector<QLineF> m_lines;
      float m_uiGraphZoomBeforeQuickZoom;
    };

  };

};

#endif // __UI_GraphView_GraphViewWidget__
