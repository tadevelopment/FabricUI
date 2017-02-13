// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPainterPathStroker>

#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/FixedPort.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Port.h>

#include <algorithm>
#include <cstdlib>

using namespace FabricUI::GraphView;

Connection::Connection(
  Graph * graph,
  ConnectionTarget * src,
  ConnectionTarget * dst,
  bool forceUseOfPinColor
  )
  : QObject( graph->itemGroup() )
  , QGraphicsPathItem( graph->itemGroup() )
  , m_graph( graph )
  , m_src( src )
  , m_dst( dst )
  , m_hovered( false )
  , m_dragging( false )
  , m_aboutToBeDeleted( false )
  , m_hasSelectedTarget( false )
{
  bool isExposedConnectionSrc = (   m_src->targetType() == TargetType_Port
                                 || m_src->targetType() == TargetType_FixedPort
                                 || m_src->targetType() == TargetType_ProxyPort );
  bool isExposedConnectionDst = (   m_dst->targetType() == TargetType_Port
                                 || m_dst->targetType() == TargetType_FixedPort
                                 || m_dst->targetType() == TargetType_ProxyPort );

  m_isExposedConnection = (   isExposedConnectionSrc
                           || isExposedConnectionDst );

  m_tooltip = "<p style='white-space:pre'>Connection between";
  {
    QString srcPortName;
    QString srcPortParent;
    QString srcPortDataType;
    QString dstPortName;
    QString dstPortParent;
    QString dstPortDataType;
    for (int i=0;i<2;i++)
    {
      ConnectionTarget *target       = (i == 0 ? src             : dst);
      QString          &portName     = (i == 0 ? srcPortName     : dstPortName);
      QString          &portParent   = (i == 0 ? srcPortParent   : dstPortParent);
      QString          &portDataType = (i == 0 ? srcPortDataType : dstPortDataType);
      switch (target->targetType())
      {
        case TargetType_Pin:
        {
          Pin &t = *(Pin *)target;
          if (!t.name()    .empty())  portName     = t.name().c_str();
          if (!t.dataType().empty())  portDataType = t.dataType().c_str();
          portParent = t.node()->name_QS() + QString(".");
        } break;
        case TargetType_Port:
        {
          Port &t = *(Port *)target;
          if (!t.name()    .empty())  portName     = t.name().c_str();
          if (!t.dataType().empty())  portDataType = t.dataType().c_str();
        } break;
        case TargetType_InstBlockPort:
        {
          InstBlockPort &t = *(InstBlockPort *)target;
          if (!t.name()    .empty())  portName     = t.name().c_str();
          if (!t.dataType().empty())  portDataType = t.dataType().c_str();
          portParent = t.instBlock()->node()->name_QS() + QString(".") + t.instBlock()->name_QS() + QString(".");
        } break;
        default:
          break;
      };
    }
    if (srcPortName    .isEmpty())  srcPortName     = "unknown";
    if (srcPortDataType.isEmpty())  srcPortDataType = "unknown";
    if (dstPortName    .isEmpty())  dstPortName     = "unknown";
    if (dstPortDataType.isEmpty())  dstPortDataType = "unknown";
    m_tooltip += QString("<br/>") + "<big><b>    - " + srcPortParent + "<font color=#0000aa>" + srcPortName + "</font>    </b></big> (data type: <b><font color=#003300>" + srcPortDataType + "</font></b>)" + "    ";
    m_tooltip += QString("<br/>and");
    m_tooltip += QString("<br/>") + "<big><b>    - " + dstPortParent + "<font color=#0000aa>" + dstPortName + "</font>    </b></big> (data type: <b><font color=#003300>" + dstPortDataType + "</font></b>)" + "    ";
    m_tooltip += QString("<p/>");
  }
  enableToolTip(m_graph->config().connectionShowTooltip);

  if(m_isExposedConnection)   m_defaultPen = m_graph->config().connectionExposePen;
  else                        m_defaultPen = m_graph->config().connectionDefaultPen;
  m_hoverPen       = m_graph->config().connectionHoverPen;
  m_clipRadius     = m_graph->config().connectionExposeRadius;
  m_shapePathWidth = 2.0f * qMax((float)m_defaultPen.widthF(), m_graph->config().connectionClickableDistance);

  QColor color = m_graph->config().connectionColor;
  if(m_graph->config().connectionUsePinColor || forceUseOfPinColor)
  {
    for(int i=0;i<2;i++)
    {
      ConnectionTarget *target = (i == 0 ? src : dst);

      if(target->targetType() == TargetType_Pin)
      {
        color = ((Pin*)target)->color();
        break;
      }
      else if(target->targetType() == TargetType_Port)
      {
        color = ((Port*)target)->color();
      }
      else if(target->targetType() == TargetType_InstBlockPort)
      {
        color = ((InstBlockPort*)target)->color();
      }
    }
  }
  setColor(color);
  setAcceptHoverEvents(true);
  setPen(m_defaultPen);

  setZValue(-1);

  dependencyMoved();
  dependencySelected();

  MainPanel *mainPanel = graph->mainPanel();
  QObject::connect(
    mainPanel, SIGNAL(geometryChanged()),
    this, SLOT(dependencyMoved())
    );
  QObject::connect(
    mainPanel, SIGNAL(canvasZoomChanged(float)),
    this, SLOT(dependencyMoved())
    );
  QObject::connect(
    mainPanel, SIGNAL(canvasPanChanged(QPointF)),
    this, SLOT(dependencyMoved())
    );

  for(int i=0;i<2;i++)
  {
    ConnectionTarget * target = src;
    if(i>0)
      target = dst;

    if(target->targetType() == TargetType_Pin)
    {
      Pin * pin = (Pin*)target;
      connect(
        pin, SIGNAL(drawStateChanged()),
        this, SLOT(dependencyMoved())
        );
      if ( i == 0 )
        QObject::connect(
          pin, SIGNAL(outCircleScenePositionChanged()),
          this, SLOT(dependencyMoved())
          );
      else
        QObject::connect(
          pin, SIGNAL(inCircleScenePositionChanged()),
          this, SLOT(dependencyMoved())
          );
      Node * node = pin->node();
      QObject::connect(node, SIGNAL(selectionChanged(FabricUI::GraphView::Node *, bool)), this, SLOT(dependencySelected()));
    }
    if(target->targetType() == TargetType_InstBlockPort)
    {
      InstBlockPort * instBlockPort = (InstBlockPort*)target;
      connect(
        instBlockPort, SIGNAL(drawStateChanged()),
        this, SLOT(dependencyMoved())
        );
      if ( i == 0 )
        QObject::connect(
          instBlockPort, SIGNAL(outCircleScenePositionChanged()),
          this, SLOT(dependencyMoved())
          );
      else
        QObject::connect(
          instBlockPort, SIGNAL(inCircleScenePositionChanged()),
          this, SLOT(dependencyMoved())
          );
    }
    else if(target->targetType() == TargetType_MouseGrabber)
    {
      MouseGrabber * grabber = (MouseGrabber*)target;
      QObject::connect(grabber, SIGNAL(positionChanged(QPointF)), this, SLOT(dependencyMoved()));
    }
    else if(target->targetType() == TargetType_Port)
    {
      Port * port = (Port*)target;
      QObject::connect(port, SIGNAL(positionChanged()), this, SLOT(dependencyMoved()));
      QObject::connect(port->sidePanel(), SIGNAL(scrolled()), this, SLOT(dependencyMoved()));
    }
    else if(target->targetType() == TargetType_FixedPort)
    {
      FixedPort * fixedPort = (FixedPort*)target;
      QObject::connect(fixedPort, SIGNAL(positionChanged()), this, SLOT(dependencyMoved()));
      QObject::connect(fixedPort->sidePanel(), SIGNAL(scrolled()), this, SLOT(dependencyMoved()));
    }
  }
}

Connection::~Connection()
{
  for (int i=0;i<2;i++)
  {
    ConnectionTarget *target = (i == 0 ? m_src : m_dst);
    if (target->targetType() == TargetType_Pin)
    {
      Pin &t = *(Pin *)target;
      t.node()->setConnectionHighlighted(false);
    }
    if (target->targetType() == QGraphicsItemType_InstBlockPort)
    {
      InstBlockPort &t = *(InstBlockPort *)target;
      t.instBlock()->node()->setConnectionHighlighted(false);
    }
  }
}

void Connection::setColor(QColor color)
{
  if(m_graph->config().connectionUsePinColor)
  {
    QColor defaultColor = m_graph->config().connectionColor;
    float ratio = m_graph->config().connectionNodePortColorRatio;
    float iratio = 1.0 - ratio;
    int r = (int)(float(color.red()) * ratio + float(defaultColor.red()) * iratio);
    int g = (int)(float(color.green()) * ratio + float(defaultColor.green()) * iratio);
    int b = (int)(float(color.blue()) * ratio + float(defaultColor.blue()) * iratio);
    color = QColor(r, g, b);
  }
  else
  {
    color = m_graph->config().connectionColor;
  }

  m_color = color;
  m_defaultPen.setColor(color);
  setPen(m_defaultPen);
}

QPointF Connection::srcPoint() const
{
  if(m_aboutToBeDeleted)
    return boundingRect().topLeft();

  return mapFromScene(m_src->connectionPos(PortType_Output));
}

QPointF Connection::dstPoint() const
{
  if(m_aboutToBeDeleted)
    return boundingRect().bottomRight();

  return mapFromScene(m_dst->connectionPos(PortType_Input));
}

void Connection::invalidate()
{
  m_aboutToBeDeleted = true;
  prepareGeometryChange();
}

void Connection::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  m_hovered = true;
  setPen(m_hoverPen);

  if (graph()->config().highlightConnectionTargets)
  {
    for (int i=0;i<2;i++)
    {
      ConnectionTarget *target = (i == 0 ? m_src : m_dst);
      if (target->targetType() == TargetType_Pin)
      {
        Pin &t = *(Pin *)target;
        t.node()->setConnectionHighlighted(m_hovered);
      }
      if (target->targetType() == QGraphicsItemType_InstBlockPort)
      {
        InstBlockPort &t = *(InstBlockPort *)target;
        t.instBlock()->node()->setConnectionHighlighted(m_hovered);
      }
    }
  }

  QGraphicsPathItem::hoverEnterEvent(event);

  if (graph()->config().highlightConnectionTargets)
    graph()->update();
}

void Connection::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  m_hovered = false;
  setPen(m_defaultPen);

  if (graph()->config().highlightConnectionTargets)
  {
    for (int i=0;i<2;i++)
    {
      ConnectionTarget *target = (i == 0 ? m_src : m_dst);
      if (target->targetType() == TargetType_Pin)
      {
        Pin &t = *(Pin *)target;
        t.node()->setConnectionHighlighted(m_hovered);
      }
      if (target->targetType() == QGraphicsItemType_InstBlockPort)
      {
        InstBlockPort &t = *(InstBlockPort *)target;
        t.instBlock()->node()->setConnectionHighlighted(m_hovered);
      }
    }
  }

  QGraphicsPathItem::hoverLeaveEvent(event);

  if (graph()->config().highlightConnectionTargets)
    graph()->update();
}

void Connection::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if( MainPanel::filterMousePressEvent( event ) )
    return event->ignore();

  if(!graph()->isEditable())
    return QGraphicsPathItem::mousePressEvent(event);

  if(event->button() == Qt::LeftButton)
  {
    m_dragging = true;
    QPointF pos = mapToScene(event->pos());
    qreal dInput = (pos - m_src->connectionPos(PortType_Output)).manhattanLength();
    qreal dOutput = (pos - m_dst->connectionPos(PortType_Input)).manhattanLength();
    m_draggingInput = dInput < dOutput;
    m_lastDragPoint = pos;
    event->accept();
  }
  else if(event->button() == Qt::MiddleButton)
  {
    if(graph()->config().middleClickDeletesConnections)
    {
      std::vector<Connection*> conns;
      conns.push_back(this);
      graph()->controller()->gvcDoRemoveConnections(conns);
      event->accept();
      return;
    }
    else
      QGraphicsPathItem::mousePressEvent(event);
  }
  else
    QGraphicsPathItem::mousePressEvent(event);
}

void Connection::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
  QMenu * menu = graph()->getConnectionContextMenu( this );
  if ( menu )
  {
    menu->exec( QCursor::pos() );
    menu->setParent( NULL );
    menu->deleteLater();
  }
}

void Connection::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if(m_dragging)
  {
    QPointF scenePos = mapToScene(event->pos());
    QPointF delta = scenePos - m_lastDragPoint;

    // todo: the disconnect threshold maybe should be a graph setting
    if(delta.manhattanLength() > 0)
    {
      // create local variables
      // since "this" might be deleted after the removeConnections call
      ConnectionTarget * src = m_src;
      ConnectionTarget * dst = m_dst;
      Graph * graph = m_graph;
      bool draggingInput = m_draggingInput;

      graph->controller()->beginInteraction();

      std::vector<Connection*> conns;
      conns.push_back(this);
      if(graph->controller()->gvcDoRemoveConnections(conns))
      {
        if(!draggingInput)
        {
          graph->constructMouseGrabber(scenePos, (Pin*)src, PortType_Input);
        }
        else
        {
          graph->constructMouseGrabber(scenePos, (Pin*)dst, PortType_Output);
        }
      }
      else
      {
        graph->controller()->endInteraction();
      }
    }
  }
  else
    QGraphicsPathItem::mouseMoveEvent(event);
}

void Connection::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  // [FE-6836] connections of IO ports are always dimmed.
  if (m_src->path() == m_dst->path() && !m_dragging && m_src->isRealPort() && m_dst->isRealPort())
  {
    painter->setOpacity(0.15);
    QGraphicsPathItem::paint(painter, option, widget);
    painter->setOpacity(1.0);
  }

  // draw dimmed connection.
  else if (m_isExposedConnection && !m_hovered && !m_hasSelectedTarget && m_graph->config().dimConnectionLines)
  {
    painter->setOpacity(0.15);
    QGraphicsPathItem::paint(painter, option, widget);
    painter->setOpacity(0.5);
    painter->setClipPath(m_clipPath);
    QGraphicsPathItem::paint(painter, option, widget);
    painter->setClipping(false);
    painter->setOpacity(1.0);
  }

  // draw regular connection.
  else
  {
    QGraphicsPathItem::paint(painter, option, widget);
  }
}

QPainterPath Connection::shape() const
{
  return m_shapePath;
}

void Connection::dependencyMoved()
{
  QPointF currSrcPoint = srcPoint();
  QPointF currDstPoint = dstPoint();
  float tangentLength = computeTangentLength();

  // painter->setRenderHint(QPainter::Antialiasing,true);
  // painter->setRenderHint(QPainter::HighQualityAntialiasing,true);

  QPainterPath path;
  path.moveTo(currSrcPoint);

  path.cubicTo(
      currSrcPoint + QPointF(tangentLength, 0), 
      currDstPoint - QPointF(tangentLength, 0), 
      currDstPoint
  );

  // we draw the curve the other way as well to
  // ensure that the polygon is closed in the top left.
  // not doing this results in an open polygon, which 
  // make the hover area for the curve very big.
  path.cubicTo(
      currDstPoint - QPointF(tangentLength, 0), 
      currSrcPoint + QPointF(tangentLength, 0), 
      currSrcPoint
  );

  setPath(path);

  QPainterPathStroker stroker;
  stroker.setWidth(m_shapePathWidth);
  m_shapePath = (stroker.createStroke(path) + path).simplified();

  if(m_isExposedConnection)
  {
    m_clipPath = QPainterPath();
    m_clipPath.addEllipse(currSrcPoint, m_clipRadius, m_clipRadius);
    m_clipPath.addEllipse(currDstPoint, m_clipRadius, m_clipRadius);
  }
}

void Connection::dependencySelected()
{
  bool oldHasSelectedTarget = m_hasSelectedTarget;
  m_hasSelectedTarget = false;

  if(m_src->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)m_src)->node();
    m_hasSelectedTarget = m_hasSelectedTarget || node->selected();
  }
  if(m_dst->targetType() == TargetType_Pin)
  {
    Node * node = ((Pin*)m_dst)->node();
    m_hasSelectedTarget = m_hasSelectedTarget || node->selected();
  }

  if ( m_hasSelectedTarget != oldHasSelectedTarget )
    update();
}

float Connection::computeTangentLength() const
{
  QPointF currSrcPoint = srcPoint();
  QPointF currDstPoint = dstPoint();

  // if the connection points are on the wrong
  // sides, we need to scale up the boundingRect
  float tangentLength = 0.0f;
  if(currSrcPoint.x() < currDstPoint.x())
  {
    tangentLength = m_graph->config().connectionFixedTangentLength;
    tangentLength += m_graph->config().connectionPercentualTangentLength * 0.01 * (currDstPoint.x() - currSrcPoint.x());
  }
  return tangentLength;
}
