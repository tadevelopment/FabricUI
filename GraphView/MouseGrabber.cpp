// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

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

#include <QtGui/QPainter>
#include <QtGui/QCursor>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsView>
#include <QtGui/QMenu>
#include <QtGui/QAction>

using namespace FabricUI::GraphView;

MouseGrabber::MouseGrabber(Graph * parent, QPointF mousePos, ConnectionTarget * target, PortType portType)
: ConnectionTarget(parent->itemGroup())
{
  m_connectionPos = mousePos;
  m_target = target;
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
  }

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

    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
      if (m_otherPortType == PortType_Input)
      {
        // 0 TargetType_Pin,
        // 1 TargetType_Port,
        // 2 TargetType_FixedPort,
        // 3 TargetType_ProxyPort,
        // 4 TargetType_MouseGrabber,
        // 5 TargetType_NodeHeader,
        // 6 TargetType_InstBlockHeader,
        // 7 TargetType_InstBlockPort
        // printf("source->targetType = %d\n", (int)source->targetType());

        if (m_target->targetType() == TargetType_Pin)
        {
          Pin  *pin     = static_cast<Pin *>( m_target );
          Node *node    = pin->node();
          Pin  *nextPin = pin;
          while (true)
          {
            nextPin = node->nextPin(nextPin->name());
            if (!nextPin || nextPin->portType() == pin->portType())
              break;
          }
          if (nextPin)
          {
            m_target = nextPin;
            m_connection->invalidate();
            graph()->scene()->removeItem(m_connection);
            m_connection->deleteLater();
            m_connection = new Connection(graph(), m_target, this, true /* forceUseOfPinColor */);
          }
          else
            ungrab = true;
        }
      }
    }
  }

  if(ungrab || (!event->modifiers().testFlag(Qt::ShiftModifier) && !event->modifiers().testFlag(Qt::ControlModifier)))
  {
    prepareGeometryChange();
    ungrabMouse();
    QGraphicsScene * scene = graph()->scene();
    graph()->resetMouseGrabber();
    m_connection->invalidate();
    scene->removeItem(m_connection);
    m_connection->deleteLater();
    // m_connection->setParent(this);
    scene->removeItem(this);
    this->deleteLater();
  }

  if(!event->modifiers().testFlag(Qt::ShiftModifier))
  {
    // begin interaction is issued by connection +/ PinCircle
    graph()->controller()->endInteraction();
  }
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
    graph()->controller()->gvcDoAddPort(
      QString::fromUtf8( pinName.data(), pinName.size() ),
      PortType_Output,
      QString::fromUtf8( dataType.data(), dataType.size() ),
      pinToConnectWith
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
    graph()->controller()->gvcDoAddPort(
      QString::fromUtf8( instBlockPortName.data(), instBlockPortName.size() ),
      PortType_Output,
      QString::fromUtf8( dataType.data(), dataType.size() ),
      instBlockPortToConnectWith
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
  setIcon( FabricUI::LoadPixmap( "DFGPlus.png" ) );
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
  QMenu *menu = new QMenu(NULL);

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
      node->canAddPorts()
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
  QMenu *menu = new QMenu(NULL);

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
