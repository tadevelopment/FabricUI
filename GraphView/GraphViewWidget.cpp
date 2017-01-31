// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "GraphViewWidget.h"

#include <QDebug>
#include <QPainter>
#include <QGLWidget>

#ifdef FABRICUI_TIMERS
  #include <Util/Timer.h>
#endif

#include <stdlib.h>

using namespace FabricUI::GraphView;

GraphViewWidget::GraphViewWidget(
  QWidget * parent,
  const GraphConfig & config,
  Graph * graph
  )
  : QGraphicsView(parent)
  , m_altWasHeldAtLastMousePress( false )
{
  setRenderHint(QPainter::Antialiasing);
  // setRenderHint(QPainter::HighQualityAntialiasing);
  setRenderHint(QPainter::TextAntialiasing);
  setOptimizationFlag(DontSavePainterState);

  setStyleSheet( "QGraphicsView { border-style: none; }" );

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  setViewportUpdateMode(SmartViewportUpdate);

  setAcceptDrops( true );

  // use opengl for rendering with multi sampling
  if(config.useOpenGL)
  {
    char const *useCanvasOpenGL = ::getenv( "FABRIC_USE_CANVAS_OPENGL" );
    if ( !!useCanvasOpenGL && !!useCanvasOpenGL[0] )
    {
      QGLFormat format;
      format.setSampleBuffers(true);
      QGLContext * context = new QGLContext(format);
      QGLWidget * glWidget = new QGLWidget(context);
      setViewport(glWidget);
    }
  }

  setGraph(graph);

  setMouseTracking(true);
}

Graph * GraphViewWidget::graph()
{
  return m_graph;
}

const Graph * GraphViewWidget::graph() const
{
  return m_graph;
}

void GraphViewWidget::setGraph(Graph * graph)
{
  m_scene = new GraphViewScene( graph );
  setScene(m_scene);

  QObject::connect(m_scene, SIGNAL(changed(const QList<QRectF> &)), this, SLOT(onSceneChanged()));
  QObject::connect(
    m_scene, SIGNAL(urlDropped(QUrl, bool, bool, QPointF)),
    this, SIGNAL(urlDropped(QUrl, bool, bool, QPointF))
    );

  m_graph = graph;
  if(m_graph)
  {
    m_graph->setGeometry(0, 0, size().width(), size().height());
    m_scene->addItem(m_graph);
    m_graph->updateOverlays(rect().width(), rect().height());
  }
}

void GraphViewWidget::resizeEvent(QResizeEvent * event)
{
  setSceneRect(0, 0, event->size().width(), event->size().height());
  if (m_graph)
  {
    m_graph->setGeometry(0, 0, event->size().width(), event->size().height());
    m_graph->updateOverlays(event->size().width(), event->size().height());
  }
}

void GraphViewWidget::mousePressEvent(QMouseEvent * event)
{
  m_altWasHeldAtLastMousePress =
    event->modifiers().testFlag( Qt::AltModifier );

  QGraphicsView::mousePressEvent(event);
}

void GraphViewWidget::mouseMoveEvent(QMouseEvent * event)
{
  m_lastEventPos = event->pos();
  QGraphicsView::mouseMoveEvent(event);
}

void GraphViewWidget::contextMenuEvent(QContextMenuEvent * event)
{
  if ( m_altWasHeldAtLastMousePress )
  {
    // [pz 20170113] FE-7941: we don't pop up a context menu if Alt was
    // held when the right mouse button was pressed

    event->accept();
    return;
  }

  QGraphicsView::contextMenuEvent(event);
}

QPoint GraphViewWidget::lastEventPos() const
{
  return m_lastEventPos;
}

void GraphViewWidget::onSceneChanged()
{
#ifdef FABRICUI_TIMERS
  Util::TimerPtr overAllTimer = Util::Timer::getTimer("FabricUI::GraphViewWidget");

  std::map<std::string, Util::TimerPtr> & timers = Util::Timer::getAllTimers();
  std::map<std::string, Util::TimerPtr>::iterator it;
  for(it = timers.begin(); it != timers.end(); it++)
  {
    QString message;
    message += it->second->title();
    if(message.left(8) != "FabricUI")
      continue;
    double elapsed = it->second->getElapsedMS();
    if(elapsed == 0.0)
      continue;
    message += " " + QString::number(elapsed, 'g', 3);
    message += "ms";
    printf("%s\n", message.toUtf8().constData());
    it->second->reset();
  }

  overAllTimer->resume();

#endif  
  emit sceneChanged();
}

bool GraphViewWidget::focusNextPrevChild(bool next)
{
  // avoid focus switching
  return false;
}

void GraphViewWidget::drawBackground(QPainter *painter, const QRectF &crect)
{
  painter->save();

  painter->fillRect(crect, m_graph->config().mainPanelBackgroundColor);

  {
    QPointF pan  = m_graph->mainPanel()->canvasPan();
    qreal   zoom = m_graph->mainPanel()->canvasZoom();

    QTransform transform;
    transform.translate(pan.rx(), pan.ry());
    transform.scale(zoom, zoom);

    QRectF rect = transform.inverted().mapRect(crect);

    int gridSize = m_graph->config().mainPanelGridSpanS;
    int left = int(rect.left()) - (int(rect.left()) % gridSize);
    int top  = int(rect.top())  - (int(rect.top())  % gridSize);

    // todo: allocate this first, the count is fixed
    std::vector<QLineF> gridLines;

    // Draw horizontal fine lines
    gridLines.clear();
    painter->setPen(m_graph->config().mainPanelGridPenS);
    float y = float(top);
    while(y < float(rect.bottom()))
    {
      gridLines.push_back(QLineF( rect.left(), y, rect.right(), y ));
      y += float(gridSize);
    }
    painter->drawLines(&gridLines[0], gridLines.size());

    // Draw vertical fine lines
    gridLines.clear();
    float x = float(left);
    while(x < float(rect.right()))
    {
      gridLines.push_back(QLineF( x, rect.top(), x, rect.bottom() ));
      x += float(gridSize);
    }
    painter->drawLines(&gridLines[0], gridLines.size());

    gridSize = m_graph->config().mainPanelGridSpanL;
    left = int(rect.left()) - (int(rect.left()) % gridSize);
    top =  int(rect.top())  - (int(rect.top())  % gridSize);

    // Draw horizontal thick lines
    gridLines.clear();
    painter->setPen(m_graph->config().mainPanelGridPenL);
    y = float(top);
    while(y < float(rect.bottom()))
    {
      gridLines.push_back(QLineF( rect.left(), y, rect.right(), y ));
      y += float(gridSize);
    }
    painter->drawLines(&gridLines[0], gridLines.size());

    // Draw vertical thick lines
    gridLines.clear();
    x = float(left);
    while(x < float(rect.right()))
    {
      gridLines.push_back(QLineF( x, rect.top(), x, rect.bottom() ));
      x += float(gridSize);
    }
    painter->drawLines(&gridLines[0], gridLines.size());
  }

  painter->restore();
}

GraphViewScene::GraphViewScene( Graph * graph ) {
  m_graph = graph;
}

void GraphViewScene::dragEnterEvent( QGraphicsSceneDragDropEvent *event )
{
  QMimeData const *mimeData = event->mimeData();
  if ( mimeData->hasUrls() )
  {
    QList<QUrl> urls = mimeData->urls();
    if ( urls.count() == 1 )
    {
      event->acceptProposedAction();
    }
  }

  if ( !event->isAccepted() )
    QGraphicsScene::dragEnterEvent( event );
}

void GraphViewScene::dropEvent( QGraphicsSceneDragDropEvent *event )
{
  QGraphicsScene::dropEvent( event );
  
  QMimeData const *mimeData = event->mimeData();
  if ( mimeData->hasUrls() )
  {
    QList<QUrl> urls = mimeData->urls();
    if ( urls.count() == 1 )
    {
      QUrl url = urls.front();
      bool ctrlPressed = event->modifiers().testFlag( Qt::ControlModifier );
      bool altPressed = event->modifiers().testFlag( Qt::AltModifier );
      emit urlDropped( url, ctrlPressed, altPressed, event->pos() );
    }
  }
}

QPointF GraphViewWidget::mapToGraph( QPoint const &globalPos ) const
{
  MainPanel *mainPanel = m_graph->mainPanel();
  QPointF pos =
    mainPanel->mapFromScene( mapToScene( mapFromGlobal( globalPos ) ) );
  QPointF pan  = mainPanel->canvasPan();
  pos -= pan;
  if ( float zoom = mainPanel->canvasZoom() )
    pos /= zoom;
  return pos;
}
