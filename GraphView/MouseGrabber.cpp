// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricServices/Persistence/RTValToJSONEncoder.hpp>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGMetaDataHelpers.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/Dialogs/DFGEditPortDialog.h>
#include <FabricUI/GraphView/Connection.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/InstBlockHeader.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/MouseGrabber.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/Util/LoadPixmap.h>
#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/NodeHeader.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/PinCircle.h>
#include <FabricUI/GraphView/FixedPort.h>

#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QAction>

using namespace FabricUI::GraphView;

MouseGrabber::MouseGrabber(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType)
: ConnectionTarget(parent->itemGroup())
  , m_lastSidePanel( NULL )
{
  m_connectionPos = mousePos;
  m_target = target;
  m_target->setHighlighted( true );
  m_otherPortType = portType;
  m_targetUnderMouse = NULL;

  const GraphConfig & config = parent->config();
  m_radius = config.mouseGrabberRadius;

  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  setPreferredSize(QSizeF(diameter(), diameter()));
  setPos(-radius(), -radius());
  setWindowFrameMargins(0, 0, 0, 0);

  QPointF localPos = parent->itemGroup()->mapFromScene(m_connectionPos);
  setZValue(-1);
  setTransform(QTransform::fromTranslate(localPos.x(), localPos.y()), false);
  grabMouse();

  if(m_otherPortType == PortType_Input)
    m_connection = new Connection(parent, m_target, this, true /* forceUseOfPinColor */);
  else
    m_connection = new Connection(parent, this, m_target, true /* forceUseOfPinColor */);
}

MouseGrabber::~MouseGrabber()
{
  if ( m_target )
    m_target->setHighlighted( false );
}

MouseGrabber * MouseGrabber::construct(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType)
{
  switch ( target->targetType() )
  {
    case TargetType_NodeHeader:
    {
      NodeHeader * header = static_cast<NodeHeader *>( target );
      Node * node = header->node();

      QMenu * menu = createNodeHeaderMenu(node, NULL, portType);
      if(menu == NULL)
        return NULL;

      QPoint globalPos = QCursor::pos();
      QAction * action = menu->exec(globalPos);

      if(action == NULL)
        return NULL;

      QString name = action->data().toString();
      if(name == "")
        return NULL;

      Pin * pin = node->pin(name.toUtf8().constData());
      if(!pin)
        return NULL;

      return construct(parent, mousePos, pin, portType);
    }
    break;

    case TargetType_InstBlockHeader:
    {
      InstBlockHeader * header = static_cast<InstBlockHeader *>( target );
      InstBlock * instBlock = header->instBlock();

      QMenu * menu = createInstBlockHeaderMenu(instBlock, NULL, portType);
      if(menu == NULL)
        return NULL;

      QPoint globalPos = QCursor::pos();
      QAction * action = menu->exec(globalPos);

      if(action == NULL)
        return NULL;

      QString name = action->data().toString();
      if(name == "")
        return NULL;

      InstBlockPort * instBlockPort =
        instBlock->instBlockPort(name.toUtf8().constData());
      if(!instBlockPort)
        return NULL;

      return construct(parent, mousePos, instBlockPort, portType);
    }
    break;

    default:
      return new MouseGrabber(parent, mousePos, target, portType);
  }
}

float MouseGrabber::radius() const
{
  return m_radius;
}

float MouseGrabber::diameter() const
{
  return m_radius * 2.0f;
}

ConnectionTarget * MouseGrabber::target()
{
  return m_target;
}

const ConnectionTarget * MouseGrabber::target() const
{
  return m_target;
}

Graph * MouseGrabber::graph()
{
  return target()->graph();
}

const Graph * MouseGrabber::graph() const
{
  return target()->graph();
}

QColor MouseGrabber::color() const
{
  return m_target->color();
}

QPointF MouseGrabber::connectionPos(PortType pType) const
{
  if(m_targetUnderMouse)
    return m_targetUnderMouse->connectionPos(m_otherPortType);
  return m_connectionPos;
}

void MouseGrabber::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  m_connectionPos = mapToScene(event->pos());
  QPointF mousePos = mapToItem(graph()->itemGroup(), event->pos());
  setTransform(QTransform::fromTranslate(mousePos.x(), mousePos.y()), false);

  emit positionChanged(mousePos);

  QList<QGraphicsItem *> items = collidingItems(Qt::IntersectsItemBoundingRect);

  bool isDraggingPortInSidePanel = false;
  if( m_lastSidePanel != NULL )
    m_lastSidePanel->onDraggingPortLeave();
  m_lastSidePanel = NULL;

  ConnectionTarget * newTargetUnderMouse = NULL;
  ConnectionTarget * prevTargetUnderMouse = m_targetUnderMouse;
  float distance = 1000000.0f;
  for(int i=0;i<items.count();i++)
  {
    if(items[i]->type() == QGraphicsItemType_PinCircle)
    {
      PinCircle * pinCircle = (PinCircle *)items[i];
      ConnectionTarget * target = pinCircle->target();
      std::string failureReason;
      bool success = false;
      if(pinCircle->portType() == PortType_Input)
        success = m_target->canConnectTo(target, failureReason);
      else
        success = target->canConnectTo(m_target, failureReason);
      if(success)
      {
        float newDistance = (pinCircle->centerInSceneCoords() - mousePos).manhattanLength();
        if(newDistance < distance)
        {
          distance = newDistance;
          newTargetUnderMouse = target;
          QToolTip::hideText();
        }
      }
      else
      {
        if(failureReason.length() > 0)
        {
          QString failureReasonQString = failureReason.c_str();
          if(!QToolTip::isVisible() || QToolTip::text() != failureReasonQString)
          {
            QGraphicsView * view = scene()->views()[0];
            if(view)
            {
              // map the position
              QPoint widgetPos = view->mapFromScene( QPoint(
                int( m_connectionPos.x() ), int( m_connectionPos.y() ) ) );
              QPoint globalPos = view->mapToGlobal(widgetPos);

              // map the exit rect. if the mouse leaves the area the tooltip will disappear
              QRectF circleRect = pinCircle->boundingRect();
              QPointF circleTopLeft = circleRect.topLeft();
              QPointF circleBottomRight = circleRect.bottomRight();
              QPointF sceneCircleTopLeft = pinCircle->mapToScene(circleTopLeft);
              QPointF sceneCircleBottomRight = pinCircle->mapToScene(circleBottomRight);
              QPoint widgetCircleTopLeft =
                view->mapFromScene( QPoint( int( sceneCircleTopLeft.x() ),
                                            int( sceneCircleTopLeft.y() ) ) );
              QPoint widgetCircleBottomRight = view->mapFromScene(
                QPoint( int( sceneCircleBottomRight.x() ),
                        int( sceneCircleBottomRight.y() ) ) );

              QRect rect(widgetCircleTopLeft, widgetCircleBottomRight);
              if(rect.contains(widgetPos))
                QToolTip::showText(globalPos, failureReasonQString, view, rect);
              else
                QToolTip::hideText();
            }
          }
        }
      }
    }
    else if (
      items[i]->type() == QGraphicsItemType_SidePanel &&
      target()->targetType() == TargetType_Port
      )
    {
      Port* port = (Port*)target();
      SidePanel* sidePanel = (SidePanel*)items[i];
      if (
        port->allowEdits() // can it be re-ordered ?
        && port->sidePanel() == sidePanel
      )
      {
        sidePanel->onDraggingPort( event, port );
        isDraggingPortInSidePanel = true;
        m_lastSidePanel = sidePanel;
      }
    }
  }

  // changing the cursor to "simulate" QDrag
  if ( isDraggingPortInSidePanel )
    setCursor( Qt::ClosedHandCursor );
  else
    setCursor( Qt::ArrowCursor );

  if(newTargetUnderMouse == NULL && prevTargetUnderMouse != NULL)
  {
    prevTargetUnderMouse->setHighlighted(false);
    prevTargetUnderMouse = NULL;
  }
  else
  {
    if(prevTargetUnderMouse != NULL && prevTargetUnderMouse != newTargetUnderMouse)
      prevTargetUnderMouse->setHighlighted(false);
    if(newTargetUnderMouse)
      newTargetUnderMouse->setHighlighted(true);
  }

  m_targetUnderMouse = newTargetUnderMouse;
}

void MouseGrabber::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  bool ungrab = false;

  if( m_lastSidePanel )
    m_lastSidePanel->onDroppingPort();

  if(m_targetUnderMouse)
  {
    m_targetUnderMouse->setHighlighted(false);

    ConnectionTarget * source;
    ConnectionTarget * target;

    if(m_otherPortType == PortType_Input)
    {
      source = m_target;
      target = m_targetUnderMouse;
    }
    else
    {
      source = m_targetUnderMouse;
      target = m_target;
    }

    invokeConnect(source, target);

    // [FE-6842]
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
      bool forceUseOfPinColor = true;
      if (m_target->targetType() == TargetType_Pin)
      {
        Pin  *p    = static_cast<Pin *>( m_target );
        Node *node = p->node();
        Pin  *next = p;
        while (true)
        {
          next = node->nextPin(next->name());
          if (!next || next->portType() == p->portType())
            break;
        }
        if (next)
        {
          m_target = next;
          m_connection->invalidate();
          graph()->scene()->removeItem(m_connection);
          m_connection->deleteLater();
          if (m_otherPortType == PortType_Input)
            m_connection = new Connection(graph(), m_target, this, forceUseOfPinColor);
          else
            m_connection = new Connection(graph(), this, m_target, forceUseOfPinColor);
        }
        else
          ungrab = true;
      }
      else if (m_target->targetType() == TargetType_Port)
      {
        Port *p    = static_cast<Port *>( m_target );
        Port *next = p;
        while (true)
        {
          next = graph()->nextPort(next->name());
          if (!next || next->portType() == p->portType())
            break;
        }
        if (next)
        {
          m_target = next;
          m_connection->invalidate();
          graph()->scene()->removeItem(m_connection);
          m_connection->deleteLater();
          if (m_otherPortType == PortType_Input)
            m_connection = new Connection(graph(), m_target, this, forceUseOfPinColor);
          else
            m_connection = new Connection(graph(), this, m_target, forceUseOfPinColor);
        }
        else
          ungrab = true;
      }
    }
  }

  if(ungrab || (!event->modifiers().testFlag(Qt::ShiftModifier) && !event->modifiers().testFlag(Qt::ControlModifier)))
    performUngrab( 0 );

  if(!event->modifiers().testFlag(Qt::ShiftModifier))
  {
    // begin interaction is issued by connection +/ PinCircle
    graph()->controller()->endInteraction();
  }
}

void MouseGrabber::performUngrab( ConnectionTarget *fromCT )
{
  prepareGeometryChange();
  ungrabMouse();
  QGraphicsScene * scene = graph()->scene();
  graph()->resetMouseGrabber();
  m_connection->invalidate();
  scene->removeItem(m_connection);

  // [FE-8406] When deleting a Node while dragging a connection from it
  // the code in ~Connection() uses the Node. Thus, we have to delete the connection
  // first and then delete the Node (see FabricUI::GraphView::Graph::removeNode)
  delete m_connection;

  // m_connection->setParent(this);
  scene->removeItem(this);
  if ( m_target == fromCT )
    m_target = 0;
  if ( m_targetUnderMouse == fromCT )
    m_targetUnderMouse = 0;
  this->deleteLater();
}

void MouseGrabber::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  QColor color = m_connection->color();
  color.setAlpha(125);
  painter->setPen(color);
  painter->drawRect(windowFrameRect());
  QGraphicsWidget::paint(painter, option, widget);
}

void MouseGrabber::invokeConnect(ConnectionTarget * source, ConnectionTarget * target)
{
  if(source->targetType() == TargetType_ProxyPort && target->targetType() == TargetType_Pin)
  {
    Pin *pinToConnectWith = static_cast<Pin *>( target );
    FTL::CStrRef pinName = pinToConnectWith->name();
    FTL::CStrRef dataType = pinToConnectWith->dataType();
    std::string metaData;
    if (graph()->controller()->gvcGetCurrentExecPath().isEmpty()) // [FE-7700]
    {
      FTL::JSONEnc<> metaDataEnc( metaData );
      FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, DFG_METADATA_UIPERSISTVALUE, "true" );
    }
    graph()->controller()->gvcDoAddPort(
      QString::fromUtf8( pinName.data(), pinName.size() ),
      PortType_Output,
      QString::fromUtf8( dataType.data(), dataType.size() ),
      pinToConnectWith,
      QString(),
      QString::fromUtf8( metaData.data(), metaData.size() )
      );
  }
  else if(target->targetType() == TargetType_ProxyPort && source->targetType() == TargetType_Pin)
  {
    Pin *pinToConnectWith = static_cast<Pin *>( source );
    FTL::CStrRef pinName = pinToConnectWith->name();
    FTL::CStrRef dataType = pinToConnectWith->dataType();
    graph()->controller()->gvcDoAddPort(
      QString::fromUtf8( pinName.data(), pinName.size() ),
      PortType_Input,
      QString::fromUtf8( dataType.data(), dataType.size() ),
      pinToConnectWith
      );
  }
  else if(source->targetType() == TargetType_ProxyPort && target->targetType() == TargetType_InstBlockPort)
  {
    InstBlockPort *instBlockPortToConnectWith = static_cast<InstBlockPort *>( target );
    FTL::CStrRef instBlockPortName = instBlockPortToConnectWith->name();
    FTL::CStrRef dataType = instBlockPortToConnectWith->dataType();
    std::string metaData;
    if (graph()->controller()->gvcGetCurrentExecPath().isEmpty()) // [FE-7700]
    {
      FTL::JSONEnc<> metaDataEnc( metaData );
      FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, DFG_METADATA_UIPERSISTVALUE, "true" );
    }
    graph()->controller()->gvcDoAddPort(
      QString::fromUtf8( instBlockPortName.data(), instBlockPortName.size() ),
      PortType_Output,
      QString::fromUtf8( dataType.data(), dataType.size() ),
      instBlockPortToConnectWith,
      QString(),
      QString::fromUtf8( metaData.data(), metaData.size() )
      );
  }
  else if(target->targetType() == TargetType_ProxyPort && source->targetType() == TargetType_InstBlockPort)
  {
    InstBlockPort *instBlockPortToConnectWith = static_cast<InstBlockPort *>( source );
    FTL::CStrRef instBlockPortName = instBlockPortToConnectWith->name();
    FTL::CStrRef dataType = instBlockPortToConnectWith->dataType();
    graph()->controller()->gvcDoAddPort(
      QString::fromUtf8( instBlockPortName.data(), instBlockPortName.size() ),
      PortType_Input,
      QString::fromUtf8( dataType.data(), dataType.size() ),
      instBlockPortToConnectWith
      );
  }
  else if(source->targetType() == TargetType_NodeHeader)
  {
    NodeHeader * header = static_cast<NodeHeader *>( source );
    Node * node = header->node();

    QPoint globalPos = QCursor::pos();
    invokeNodeHeaderMenu(node, target, PortType_Input, globalPos);
  }
  else if(target->targetType() == TargetType_NodeHeader)
  {
    NodeHeader * header = static_cast<NodeHeader *>( target );
    Node * node = header->node();

    QPoint globalPos = QCursor::pos();
    invokeNodeHeaderMenu(node, source, PortType_Output, globalPos);
  }
  else if(source->targetType() == TargetType_InstBlockHeader)
  {
    InstBlockHeader * header = static_cast<InstBlockHeader *>( source );
    InstBlock * instBlock = header->instBlock();

    QPoint globalPos = QCursor::pos();
    invokeInstBlockHeaderMenu(instBlock, target, PortType_Input, globalPos);
  }
  else if(target->targetType() == TargetType_InstBlockHeader)
  {
    InstBlockHeader * header = static_cast<InstBlockHeader *>( target );
    InstBlock * instBlock = header->instBlock();

    QPoint globalPos = QCursor::pos();
    invokeInstBlockHeaderMenu(instBlock, source, PortType_Output, globalPos);
  }
  else
  {
    std::vector<ConnectionTarget  *> sources;
    std::vector<ConnectionTarget  *> targets;
    sources.push_back(source);
    targets.push_back(target);
    graph()->controller()->gvcDoAddConnections(sources, targets);
  }
}

static void GetDesiredPortNameAndTypeSpecForConnectionTarget(
  ConnectionTarget *target,
  FTL::StrRef &desiredPortNameStr,
  FTL::StrRef &typeSpecStr
  )
{
  if ( !target )
    return;

  switch ( target->targetType() )
  {
    case TargetType_Pin:
    {
      Pin *targetPin = static_cast<Pin *>( target );
      desiredPortNameStr = targetPin->name();
      typeSpecStr = targetPin->dataType();
    }
    break;

    case TargetType_Port:
    {
      Port *targetPort = static_cast<Port *>( target );
      desiredPortNameStr = targetPort->name();
      typeSpecStr = targetPort->dataType();
    }
    break;

    case TargetType_FixedPort:
    {
      FixedPort *targetFixedPort = static_cast<FixedPort *>( target );
      desiredPortNameStr = targetFixedPort->name();
      typeSpecStr = targetFixedPort->dataType();
    }
    break;

    case TargetType_InstBlockPort:
    {
      InstBlockPort *targetInstBlockPort = static_cast<InstBlockPort *>( target );
      desiredPortNameStr = targetInstBlockPort->name();
      typeSpecStr = targetInstBlockPort->dataType();
    }
    break;

    default:
      assert( false );
      break;
  }
}

ExposePortAction::ExposePortAction(
  QObject *parent,
  FabricUI::DFG::DFGController *dfgController,
  ConnectionTarget *other,
  PortType connectionPortType
  )
  : QAction( parent )
  , m_dfgController( dfgController )
  , m_other( other )
  , m_connectionPortType( connectionPortType )
{
  setText( "Expose new port" );
  setIcon( FabricUI::LoadPixmap( "DFGPlus.png" ).scaledToWidth( 20, Qt::SmoothTransformation ) );
  connect(
    this, SIGNAL(triggered()),
    this, SLOT(onTriggered())
    );
}

void ExposePortAction::onTriggered()
{
  FTL::StrRef defaultDesiredPortNameStr;
  FTL::StrRef defaultTypeSpecStr;
  GetDesiredPortNameAndTypeSpecForConnectionTarget(
    m_other,
    defaultDesiredPortNameStr,
    defaultTypeSpecStr
    );

  FabricUI::DFG::DFGEditPortDialog dialog(
    m_dfgController->getDFGWidget(),
    m_dfgController->getClient(),
    allowNonInPortType(), // showPortType
    true, // canEditPortType
    m_dfgController->getDFGWidget()->getConfig(),
    true
    );

  dialog.setTitle(
    QString::fromUtf8(
      defaultDesiredPortNameStr.data(),
      defaultDesiredPortNameStr.size()
      )
    );
  dialog.setDataType(
    QString::fromUtf8(
      defaultTypeSpecStr.data(),
      defaultTypeSpecStr.size()
      )
    );
  if ( m_connectionPortType == FabricUI::GraphView::PortType_Output )
    dialog.setPortType("In");
  else
    dialog.setPortType("Out");

  if ( dialog.exec() != QDialog::Accepted )
    return;

  std::string metaData;
  {
    FTL::JSONEnc<> metaDataEnc( metaData );
    FTL::JSONObjectEnc<> metaDataObjectEnc( metaDataEnc );
    if(dialog.hidden())
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiHidden", "true" );
    if(dialog.opaque())
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiOpaque", "true" );

    if(dialog.persistValue())
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, DFG_METADATA_UIPERSISTVALUE, "true" );

    if(dialog.hasSoftRange())
    {
      QString range = "(" + QString::number(dialog.softRangeMin()) + ", " + QString::number(dialog.softRangeMax()) + ")";
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", range.toUtf8().constData() );
    } else
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiRange", "" );

    if(dialog.hasHardRange())
    {
      QString range = "(" + QString::number(dialog.hardRangeMin()) + ", " + QString::number(dialog.hardRangeMax()) + ")";
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", range.toUtf8().constData() );
    } else
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiHardRange", "" );

    if(dialog.hasCombo())
    {
      QStringList combo = dialog.comboValues();
      QString flat = "(";
      for(int i=0;i<combo.length();i++)
      {
        if(i > 0)
          flat += ", ";
        flat += "\"" + combo[i] + "\"";
      }
      flat += ")";
      FabricUI::DFG::DFGAddMetaDataPair( metaDataObjectEnc, "uiCombo", flat.toUtf8().constData() );
    }
  }

  QString desiredPortName = dialog.title();
  QString typeSpec = dialog.dataType();
  QString extDep = dialog.extension();
  QString dialogPortType = dialog.portType();
  FabricCore::DFGPortType portType = FabricCore::DFGPortType_Out;
  if ( dialogPortType.isEmpty() || dialogPortType == "In" )
    portType = FabricCore::DFGPortType_In;
  else if ( dialogPortType == "IO" )
    portType = FabricCore::DFGPortType_IO;

  if(metaData == "{}")
    metaData = "";

  invokeAddPort(
    desiredPortName,
    portType,
    typeSpec,
    extDep,
    QString::fromUtf8( metaData.data(), metaData.size() )
    );
}

ExposeInstPortAction::ExposeInstPortAction(
  QObject *parent,
  FabricUI::DFG::DFGController *dfgController,
  Node *node,
  ConnectionTarget *other,
  PortType portType
  )
  : ExposePortAction( parent, dfgController, other, portType )
  , m_node( node )
{
}

void ExposeInstPortAction::invokeAddPort(
  QString desiredPortName,
  FabricCore::DFGPortType portType,
  QString typeSpec,
  QString extDep,
  QString metaData
  )
{
  FabricUI::DFG::DFGUICmdHandler *cmdHandler =
    m_dfgController->getCmdHandler();
  switch ( m_node->getNodeType() )
  {
    case Node::NodeType_Inst:
      cmdHandler->dfgDoAddInstPort(
        m_dfgController->getBinding(),
        m_dfgController->getExecPath_QS(),
        m_dfgController->getExec(),
        m_node->name_QS(),
        desiredPortName,
        portType,
        typeSpec,
        m_other? m_other->path_QS(): QString(),
        PortTypeToDFGPortType( m_connectionPortType ),
        extDep,
        metaData
        );
      break;

    case Node::NodeType_Block:
      cmdHandler->dfgDoAddBlockPort(
        m_dfgController->getBinding(),
        m_dfgController->getExecPath_QS(),
        m_dfgController->getExec(),
        m_node->name_QS(),
        desiredPortName,
        portType,
        typeSpec,
        m_other? m_other->path_QS(): QString(),
        PortTypeToDFGPortType( m_connectionPortType ),
        extDep,
        metaData
        );
      break;

    default:
      assert( false );
      break;
  }
}

QMenu * MouseGrabber::createNodeHeaderMenu(Node * node, ConnectionTarget * other, PortType nodeRole)
{
  FabricUI::DFG::DFGController * dfgController = static_cast<FabricUI::DFG::DFGController *>(node->graph()->controller());
  QMenu *menu = new QMenu(dfgController->getDFGWidget());

  // go through all the node's pins and add
  // those to the menu that can be connected.
  for(unsigned int i=0;i<node->pinCount();i++)
  {
    Pin *pin = node->pin(i);

    if(nodeRole == PortType_Output && pin->portType() == PortType_Output)
      continue; // skip this pin (an output port cannot be connected with another output port).

    if (other)
    {
      if (nodeRole == PortType_Output)
      {
        std::string failureReason;
        if (!other->canConnectTo(pin, failureReason))
          continue; // skip this pin (it cannot be connected)
      }
      else if (nodeRole == PortType_Input)
      {
        std::string failureReason;
        if (!pin->canConnectTo(other, failureReason))
          continue; // skip this pin (it cannot be connected)
      }
    }

    // construct the label for the menu.
    QString name = pin->name().c_str();
    QString label;
    if (nodeRole == PortType_Input)
    {
      if (pin->portType() == PortType_Input)
        label = name + " =";
      else
        label = name + " >";
    }
    else
    {
      label = "> " + name;
    }

    // create an action using our label and add it to the menu.
    QAction * action = new QAction(label, NULL);
    action->setData(name);
    menu->addAction(action);
  }

  menu->addSeparator();

  QAction *exposeNewPortAction =
    new ExposeInstPortAction(
      menu,
      static_cast<FabricUI::DFG::DFGController *>(
        node->graph()->controller()
        ),
      node,
      other,
      nodeRole
      );
  exposeNewPortAction->setEnabled(
      node->canEdit()
    && ( !other || other->isRealPort() )
    );
  menu->addAction( exposeNewPortAction );

  // done.
  return menu;
}

ExposeInstBlockPortAction::ExposeInstBlockPortAction(
  QObject *parent,
  FabricUI::DFG::DFGController *dfgController,
  InstBlock *instBlock,
  ConnectionTarget *other
  )
  : ExposePortAction( parent, dfgController, other, PortType_Output )
  , m_instBlock( instBlock )
{
}

void ExposeInstBlockPortAction::invokeAddPort(
  QString desiredPortName,
  FabricCore::DFGPortType portType,
  QString typeSpec,
  QString extDep,
  QString metaData
  )
{
  assert( portType == FabricCore::DFGPortType_In );
  FabricUI::DFG::DFGUICmdHandler *cmdHandler =
    m_dfgController->getCmdHandler();
  cmdHandler->dfgDoAddInstBlockPort(
    m_dfgController->getBinding(),
    m_dfgController->getExecPath_QS(),
    m_dfgController->getExec(),
    m_instBlock->node()->name_QS(),
    m_instBlock->name_QS(),
    desiredPortName,
    typeSpec,
    m_other? m_other->path_QS(): QString(),
    extDep,
    metaData
    );
}

QMenu *MouseGrabber::createInstBlockHeaderMenu(
  InstBlock *instBlock,
  ConnectionTarget *other,
  PortType nodeRole
  )
{
  FabricUI::DFG::DFGController * dfgController = static_cast<FabricUI::DFG::DFGController *>(instBlock->node()->graph()->controller());
  QMenu *menu = new QMenu(dfgController->getDFGWidget());

  // go through all the node's pins and add
  // those to the menu that can be connected.
  for(unsigned int i=0;i<instBlock->instBlockPortCount();i++)
  {
    InstBlockPort *instBlockPort = instBlock->instBlockPort(i);

    if(nodeRole == PortType_Output && instBlockPort->portType() == PortType_Output)
      continue; // skip this instBlockPort (an output port cannot be connected with another output port).

    if (other)
    {
      if (nodeRole == PortType_Output)
      {
        std::string failureReason;
        if (!other->canConnectTo(instBlockPort, failureReason))
          continue; // skip this instBlockPort (it cannot be connected)
      }
      else if (nodeRole == PortType_Input)
      {
        std::string failureReason;
        if (!instBlockPort->canConnectTo(other, failureReason))
          continue; // skip this instBlockPort (it cannot be connected)
      }
    }

    // construct the label for the menu.
    QString name = instBlockPort->name().c_str();
    QString label;
    if (nodeRole == PortType_Input)
    {
      if (instBlockPort->portType() == PortType_Input)
        label = name + " =";
      else
        label = name + " >";
    }
    else
    {
      label = "> " + name;
    }

    // create an action using our label and add it to the menu.
    QAction * action = new QAction(label, NULL);
    action->setData(name);
    menu->addAction(action);
  }

  menu->addSeparator();

  QAction *exposeNewPortAction =
    new ExposeInstBlockPortAction(
      menu,
      static_cast<FabricUI::DFG::DFGController *>(
        instBlock->node()->graph()->controller()
        ),
      instBlock,
      other
      );
  exposeNewPortAction->setEnabled(
      nodeRole == PortType_Output
    && ( !other || other->isRealPort() )
    );
  menu->addAction( exposeNewPortAction );

  // done.
  return menu;
}

void MouseGrabber::invokeNodeHeaderMenu(Node * node, ConnectionTarget * other, PortType nodeRole, QPoint pos)
{
  QMenu * menu = createNodeHeaderMenu(node, other, nodeRole);
  if(menu == NULL)
    return;

  QAction * action = menu->exec(pos);
  if(action == NULL)
    return;

  QString name = action->data().toString();
  if(name == "")
    return;

  Pin * pin = node->pin(name.toUtf8().constData());
  if(!pin)
    return;

  if(other == NULL)
  {
    // this is invoked from MouseGrabber::construct
    other = pin;
  }
  else if(nodeRole == PortType_Output)
    invokeConnect(other, pin);
  else
    invokeConnect(pin, other);
}

void MouseGrabber::invokeInstBlockHeaderMenu(
  InstBlock * instBlock,
  ConnectionTarget * other,
  PortType instBlockRole,
  QPoint pos
  )
{
  QMenu * menu = createInstBlockHeaderMenu(instBlock, other, instBlockRole);
  if(menu == NULL)
    return;

  QAction * action = menu->exec(pos);
  if(action == NULL)
    return;

  QString name = action->data().toString();
  if(name == "")
    return;

  InstBlockPort * instBlockPort = instBlock->instBlockPort(name.toUtf8().constData());
  if(!instBlockPort)
    return;

  if(other == NULL)
  {
    // this is invoked from MouseGrabber::construct
    other = instBlockPort;
  }
  else if(instBlockRole == PortType_Output)
    invokeConnect(other, instBlockPort);
  else
    invokeConnect(instBlockPort, other);
}
