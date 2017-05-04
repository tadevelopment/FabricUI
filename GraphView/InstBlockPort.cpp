//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/PinCircle.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/InstBlockHeader.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/ProxyPort.h>
#include <FabricUI/GraphView/Controller.h>
#include <FabricUI/GraphView/MainPanel.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/PinCircle.h>

#include <QGraphicsLinearLayout>

// sqrt
#include <math.h>

using namespace FabricUI::GraphView;

class InstBlockPortLabel : public NodeLabel
{
  InstBlockPort * m_pin;

public:
  InstBlockPortLabel(
    InstBlockPort * pin,
    Node* node,
    QString const &text,
    QColor color,
    QColor highlightColor,
    QFont font
  ) : NodeLabel(
    pin,
    node,
    text,
    color,
    highlightColor,
    font
  ), m_pin( pin )
  {
    setEditable( false );
    setAcceptHoverEvents( true );
  }

protected:

  PinCircle* pinCircle() const
  {
    switch ( m_pin->portType() )
    {
    case PortType_Input: return m_pin->inCircle();
    case PortType_Output: return m_pin->outCircle();
    default: return NULL;
    }
  }

  void mousePressEvent( QGraphicsSceneMouseEvent* event ) FTL_OVERRIDE
  {
    if( MainPanel::filterMousePressEvent( event ) )
      return event->ignore();

    // Creating connections from Labels
    PinCircle * circle = pinCircle();
    if ( circle )
      circle->mousePressEvent( event );
    NodeLabel::mousePressEvent( event );
  }

  void hoverEnterEvent( QGraphicsSceneHoverEvent * event ) FTL_OVERRIDE
  {
    PinCircle * circle = pinCircle();
    if ( circle )
      circle->onHoverEnter();
    setHighlighted( true );
    NodeLabel::hoverEnterEvent( event );
  }

  void hoverLeaveEvent( QGraphicsSceneHoverEvent * event ) FTL_OVERRIDE
  {
    PinCircle * circle = pinCircle();
    if ( circle )
      circle->onHoverLeave();
    setHighlighted( false );
    NodeLabel::hoverLeaveEvent( event );
  }
};

InstBlockPort::InstBlockPort(
  InstBlock *instBlock,
  FTL::StrRef name,
  PortType pType,
  QColor color
  )
  : ConnectionTarget( instBlock )
  , m_instBlock( instBlock )
  , m_name( name.data(), name.size() )
  , m_labelCaption( name.data(), name.size() )
{
  m_portType = pType;
  m_color = color;
  m_index = 0;

  // [pzion 20160425] Force setDrawState to work
  m_drawState = false;
  setDrawState( true );

  const GraphConfig & config = node()->graph()->config();

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();

  if ( portType() == PortType_Output )
    layout->setContentsMargins(config.nodeWidthReduction * 0.5, 0, 0, 0);
  else
    layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(config.pinLabelSpacing);
  layout->setOrientation(Qt::Horizontal);
  setLayout(layout);

  setContentsMargins( 0, 0, 0, 3 );

  m_inCircle = NULL;
  m_outCircle = NULL;

  Node *parentNode = node();

  if(portType() == PortType_Input || portType() == PortType_IO)
  {
    m_inCircle = new PinCircle(this, PortType_Input, m_color);
    layout->addItem(m_inCircle);
    layout->setAlignment(m_inCircle, Qt::AlignLeft | Qt::AlignVCenter);

    QGraphicsItem *graphicsItem = m_inCircle;
    while ( graphicsItem )
    {
      if ( QGraphicsObject *graphicsObject = graphicsItem->toGraphicsObject() )
      {
        connect(
          graphicsObject, SIGNAL(xChanged()),
          this, SIGNAL(inCircleScenePositionChanged())
          );
        connect(
          graphicsObject, SIGNAL(yChanged()),
          this, SIGNAL(inCircleScenePositionChanged())
          );
      }
      if ( graphicsItem == parentNode )
        break;
      graphicsItem = graphicsItem->parentItem();
    }
  }

  if(portType() != PortType_Input)
  {
    layout->addStretch(1);

    if(config.nodePinStretch > 0.0f && portType() != PortType_IO)
    {
      QGraphicsWidget * stretch = new QGraphicsWidget(this);
      stretch->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
      stretch->setMinimumWidth(portType() == PortType_IO ? config.nodePinStretch * 0.5 : config.nodePinStretch);
      stretch->setMaximumHeight(1.0f);
      layout->addItem(stretch);
    }
  }

  m_label = new InstBlockPortLabel(
    this,
    parentNode,
    QSTRING_FROM_STL_UTF8(m_labelCaption),
    config.pinFontColor,
    config.pinFontHighlightColor,
    config.pinFont
    );
  layout->addItem(m_label);
  layout->setAlignment(m_label, Qt::AlignHCenter | Qt::AlignVCenter);

  if(portType() != PortType_Output)
  {
    if(config.nodePinStretch > 0.0f && portType() != PortType_IO)
    {
      QGraphicsWidget * stretch = new QGraphicsWidget(this);
      stretch->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
      stretch->setMinimumWidth(portType() == PortType_IO ? config.nodePinStretch * 0.5 : config.nodePinStretch);
      stretch->setMaximumHeight(1.0f);
      layout->addItem(stretch);
    }

    layout->addStretch(1);
  }

  m_outCircle = new PinCircle(this, PortType_Output, m_color);
  layout->addItem(m_outCircle);
  layout->setAlignment(m_outCircle, Qt::AlignRight | Qt::AlignVCenter);
  setDaisyChainCircleVisible(false);
  QGraphicsItem const *graphicsItem = m_outCircle;
  while ( graphicsItem )
  {
    if ( QGraphicsObject const *graphicsObject = graphicsItem->toGraphicsObject() )
    {
      connect(
        graphicsObject, SIGNAL(xChanged()),
        this, SIGNAL(outCircleScenePositionChanged())
        );
      connect(
        graphicsObject, SIGNAL(yChanged()),
        this, SIGNAL(outCircleScenePositionChanged())
        );
    }
    if ( graphicsItem == parentNode )
      break;
    graphicsItem = graphicsItem->parentItem();
  }

  graphicsItem = instBlock->header()->inCircle();
  while ( graphicsItem )
  {
    if ( QGraphicsObject const *graphicsObject = graphicsItem->toGraphicsObject() )
    {
      connect(
        graphicsObject, SIGNAL(xChanged()),
        this, SIGNAL(inCircleScenePositionChanged())
        );
      connect(
        graphicsObject, SIGNAL(yChanged()),
        this, SIGNAL(inCircleScenePositionChanged())
        );
    }
    if ( graphicsItem == parentNode )
      break;
    graphicsItem = graphicsItem->parentItem();
  }

  graphicsItem = instBlock->header()->outCircle();
  while ( graphicsItem )
  {
    if ( QGraphicsObject const *graphicsObject = graphicsItem->toGraphicsObject() )
    {
      connect(
        graphicsObject, SIGNAL(xChanged()),
        this, SIGNAL(outCircleScenePositionChanged())
        );
      connect(
        graphicsObject, SIGNAL(yChanged()),
        this, SIGNAL(outCircleScenePositionChanged())
        );
    }
    if ( graphicsItem == parentNode )
      break;
    graphicsItem = graphicsItem->parentItem();
  }
}

Node *InstBlockPort::node()
{
  return instBlock()->node();
}

Node const *InstBlockPort::node() const
{
  return instBlock()->node();
}

Graph *InstBlockPort::graph()
{
  return node()->graph();
}

Graph const *InstBlockPort::graph() const
{
  return node()->graph();
}

std::string InstBlockPort::path() const
{
  std::string result = instBlock()->path();
  result += graph()->config().pathSep;
  result += m_name;
  return result;
}

char const * InstBlockPort::label() const
{
  return m_labelCaption.c_str();
}

PortType InstBlockPort::portType() const
{
  return m_portType;
}

QColor InstBlockPort::color() const
{
  return m_color;
}

void InstBlockPort::setColor(QColor color, bool quiet, bool performUpdate)
{
  if(inCircle())
    inCircle()->setColor(color);
  if(outCircle())
    outCircle()->setColor(color);
  if(m_color != color)
  {
    m_color = color;
    if(!quiet)
      emit colorChanged(this, color);
  }
  if(performUpdate)
    update();
}

int InstBlockPort::index() const
{
  return m_index;
}

void InstBlockPort::setIndex(int i)
{
  m_index = i;
}

bool InstBlockPort::highlighted() const
{
  return m_highlighted;
}

void InstBlockPort::setHighlighted(bool state)
{
  m_highlighted = state;
  if(inCircle())
    inCircle()->setHighlighted(state);
  if(outCircle())
    outCircle()->setHighlighted(state);
  if(m_label)
    m_label->setHighlighted(state);
  setColor(m_color, true);
}

void InstBlockPort::setDataType(FTL::CStrRef dataType)
{
  m_dataType = dataType;
  setToolTip(dataType.c_str());

  // automatically change the label for array pins
  if(m_label)
  {
    if(m_dataType.length() > 2)
    {
      if(m_dataType.substr(m_dataType.length()-2) == "[]" && m_labelSuffix != "[]")
      {
        m_labelSuffix = "[]";
        m_label->setText( QSTRING_FROM_STL_UTF8( m_labelCaption ) );
        m_label->setSuffix( QSTRING_FROM_STL_UTF8( m_labelSuffix ) );
        return;
      }
    }
    if(m_labelSuffix.length() > 0)
    {
      m_labelSuffix = "";
      m_label->setText( QSTRING_FROM_STL_UTF8( m_labelCaption ) );
      m_label->setSuffix( QSTRING_FROM_STL_UTF8( m_labelSuffix ) );
    }
  }
}

PinCircle * InstBlockPort::inCircle()
{
  return m_inCircle;
}

const PinCircle * InstBlockPort::inCircle() const
{
  return m_inCircle;
}

PinCircle * InstBlockPort::outCircle()
{
  return m_outCircle;
}

const PinCircle * InstBlockPort::outCircle() const
{
  return m_outCircle;
}

PinCircle * InstBlockPort::findPinCircle( QPointF pos )
{
  PinCircle * circle = pos.x() < size().width() * 0.5 ? inCircle() : outCircle();
  if ( circle )
  {
    float pinClickableDistance = graph()->config().pinClickableDistance;
    QPointF center = circle->centerInSceneCoords();
    QPointF clicked = mapToScene( pos );
    float x = center.x() - clicked.x();
    float y = center.y() - clicked.y();
    float distance = sqrt( x * x + y * y );
    distance /= graph()->mainPanel()->canvasZoom();
    if ( distance > pinClickableDistance )
      circle = NULL;
  }
  return circle;
}

bool InstBlockPort::canConnectTo(
  ConnectionTarget * other,
  std::string &failureReason
  ) const
{
  switch(other->targetType())
  {
    case TargetType_InstBlockPort:
    {
      InstBlockPort * otherInstBlockPort = (InstBlockPort *)other;
      if ( this == otherInstBlockPort
        || otherInstBlockPort->node() == this->node()
        || otherInstBlockPort->portType() == PortType_Output )
        return false;
      return graph()->controller()->canConnectTo(
        path().c_str(), otherInstBlockPort->path().c_str(), failureReason
        );
    }
    case TargetType_Pin:
    {
      Pin * otherPin = (Pin *)other;
      if ( this == other
        || otherPin->node() == this->node()
        || otherPin->portType() == PortType_Output )
        return false;
      return graph()->controller()->canConnectTo(
        path().c_str(), otherPin->path().c_str(), failureReason
        );
    }
    case TargetType_Port:
    {
      Port * otherPort = (Port *)other;
      if ( this == other
        || otherPort->portType() == PortType_Output )
        return false;
      return graph()->controller()->canConnectTo(
        path().c_str(), otherPort->path().c_str(), failureReason
        );
    }
    case TargetType_ProxyPort:
    {
      ProxyPort * otherProxyPort = (ProxyPort *)other;
      if ( this == other
        || otherProxyPort->portType() == PortType_Output )
        return false;
      return true;
    }
    case TargetType_NodeHeader:
      return true;
    case TargetType_InstBlockHeader:
      return static_cast<InstBlockHeader *>( other )->instBlock() != m_instBlock;
    default:
      return false;
  }
}

QPointF InstBlockPort::connectionPos(PortType pType) const
{
  PinCircle const *pinCircle;
  if ( !drawState() )
  {
    if ( pType == PortType_Input )
      pinCircle = m_instBlock->header()->inCircle();
    else
      pinCircle = m_instBlock->header()->outCircle();
  }
  else
  {
    if ( pType == PortType_Input )
      pinCircle = inCircle();
    else
      pinCircle = outCircle();
  }

  if ( pinCircle )
    return pinCircle->centerInSceneCoords();
  else
    return QPointF();
}

void InstBlockPort::setDrawState(bool flag)
{
  if ( m_drawState != flag )
  {
    m_drawState = flag;
    setVisible( m_drawState );

    if ( m_drawState )
    {
      setMaximumHeight( 1000 );
      setSizePolicy(
        QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed )
        );
    }
    else
    {
      setMaximumHeight( 0 );
      setSizePolicy(
        QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed )
        );
    }

    emit drawStateChanged();
  }
}

void InstBlockPort::setDaisyChainCircleVisible(bool flag)
{
  if( portType() == PortType_Input )
    m_outCircle->setDaisyChainCircleVisible( flag );
}

void InstBlockPort::setName( FTL::StrRef newName )
{
  if ( newName != m_name )
  {
    bool labelIsName = m_labelCaption == m_name;
    m_name = newName;
    if ( labelIsName )
      m_labelCaption = newName;
    m_label->setText( QSTRING_FROM_STL_UTF8( m_labelCaption ) );
  }
}

bool InstBlockPort::selected() const
{
  return node()->selected();
}

void InstBlockPort::setFontColor( QColor color )
{
  m_label->setColor( color, m_label->highlightColor() );
}
