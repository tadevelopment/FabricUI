// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/FixedPort.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/QGraphicsPixmapLayoutItem.h>
#include <FabricUI/GraphView/SidePanel.h>
#include <FabricUI/Util/LoadPixmap.h>

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QLabel>

using namespace FabricUI::GraphView;

FixedPort::FixedPort(
  SidePanel * parent,
  FTL::StrRef name,
  PortType portType,
  bool portIsIO,
  FTL::StrRef dataType,
  QColor color,
  FTL::StrRef label
  )
  : ConnectionTarget( parent->itemGroup() )
  , m_sidePanel( parent )
  , m_name( name )
  , m_labelCaption( !label.empty()? label: name )
  , m_allowEdits( true )
{
  // if(parent->graph()->path().length() > 0)
  //   m_path = parent->graph()->path() + parent->graph()->config().pathSep + m_path;
  m_portType = portType;
  m_dataType = dataType;
  m_color = color;
  m_index = 0;

  init(portIsIO);
}

void FixedPort::init(bool portIsIO)
{
  setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

  const GraphConfig & config = graph()->config();

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(config.sidePanelPortLabelSpacing);
  layout->setOrientation(Qt::Horizontal);
  setLayout(layout);

  QGraphicsPixmapLayoutItem *lockItem =
    new QGraphicsPixmapLayoutItem(
      FabricUI::LoadPixmap( "fixed-port-lock.png" )
      );
  QGraphicsPixmapLayoutItem *ioItem = NULL;
  if (portIsIO)
    ioItem = new QGraphicsPixmapLayoutItem(
               FabricUI::LoadPixmap( "io-port.png" )
               );
  m_label = new PortLabel(
    this,
    QSTRING_FROM_STL_UTF8(m_labelCaption),
    config.sidePanelFontColor,
    config.sidePanelFontHighlightColor,
    config.sidePanelFont
    );
  m_circle = new PinCircle(this, m_portType, color());

  if(m_portType == PortType_Input)
  {
    layout->addItem(m_circle);
    layout->setAlignment(m_circle, Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addItem( lockItem );
    layout->setAlignment(lockItem, Qt::AlignHCenter | Qt::AlignVCenter);
    if (portIsIO)
    {
      layout->addItem( ioItem );
      layout->setAlignment(ioItem, Qt::AlignHCenter | Qt::AlignVCenter);
    }
    layout->addItem(m_label);
    layout->setAlignment(m_label, Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addStretch(1);
  }
  else if(m_portType == PortType_Output)
  {
    layout->addStretch(1);
    layout->addItem(m_label);
    layout->setAlignment(m_label, Qt::AlignHCenter | Qt::AlignVCenter);
    if (portIsIO)
    {
      layout->addItem( ioItem );
      layout->setAlignment(ioItem, Qt::AlignHCenter | Qt::AlignVCenter);
    }
    layout->addItem( lockItem );
    layout->setAlignment(lockItem, Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addItem(m_circle);
    layout->setAlignment(m_circle, Qt::AlignHCenter | Qt::AlignVCenter);
  }
}

Graph *FixedPort::graph()
{
  return sidePanel()->graph();
}

Graph const *FixedPort::graph() const
{
  return sidePanel()->graph();
}

void FixedPort::setName( FTL::CStrRef name )
{
  bool labelUsesName = m_name == m_labelCaption;
  m_name = name;
  if(labelUsesName)
    setLabel(name.c_str());
  else
    update();

  emit contentChanged();
}

char const * FixedPort::label() const
{
  return m_labelCaption.c_str();
}

void FixedPort::setLabel(char const * n)
{
  m_labelCaption = n;
  m_label->setText(QSTRING_FROM_STL_UTF8(m_labelCaption));
  update();
}

QColor FixedPort::color() const
{
  return m_color;
}

PortType FixedPort::portType() const
{
  return m_portType;
}

void FixedPort::setDataType(FTL::CStrRef dataType)
{
  m_dataType = dataType;
  setToolTip(m_dataType.c_str());
}

void FixedPort::setColor(QColor color)
{
  m_color = color;
  m_circle->setColor(m_color);
}

bool FixedPort::highlighted() const
{
  return m_highlighted;
}

void FixedPort::setHighlighted(bool state)
{
  if(m_highlighted != state)
  {
    m_circle->setHighlighted(state);
    m_label->setHighlighted(state);
    m_highlighted = state;
  }
}

bool FixedPort::canConnectTo(
  ConnectionTarget * other,
  std::string &failureReason
  ) const
{
  switch(other->targetType())
  {
    case TargetType_Pin:
    {
      Pin * otherPin = (Pin *)other;
      if ( this == other
        || portType() == PortType_Input
        || otherPin->portType() == PortType_Output )
        return false;
      return m_sidePanel->graph()->controller()->canConnectTo(
        path().c_str(),
        otherPin->path().c_str(),
        failureReason
        );
    }
    case TargetType_Port:
    {
      Port * otherPort = (Port *)other;
      if ( portType() == PortType_Input
        || otherPort->portType() == PortType_Output )
        return false;
      if(path() == otherPort->path())
        return false;
      return m_sidePanel->graph()->controller()->canConnectTo(
        path().c_str(),
        otherPort->path().c_str(),
        failureReason
        );
    }
    case TargetType_FixedPort:
    {
      FixedPort * otherFixedPort = (FixedPort *)other;
      if ( this == otherFixedPort
        || portType() == PortType_Input
        || otherFixedPort->portType() == PortType_Output )
        return false;
      if(path() == otherFixedPort->path())
        return false;
      return m_sidePanel->graph()->controller()->canConnectTo(
        path().c_str(),
        otherFixedPort->path().c_str(),
        failureReason
        );
    }
    case TargetType_InstBlockPort:
    {
      InstBlockPort * otherInstBlockPort = (InstBlockPort *)other;
      if ( portType() == PortType_Input
        || otherInstBlockPort->portType() == PortType_Output )
        return false;
      if(path() == otherInstBlockPort->path())
        return false;
      return m_sidePanel->graph()->controller()->canConnectTo(
        path().c_str(),
        otherInstBlockPort->path().c_str(),
        failureReason
        );
    }
    case TargetType_NodeHeader:
    case TargetType_InstBlockHeader:
    {
      return true;
    }
    default:
      return false;
  }
}

QPointF FixedPort::connectionPos(PortType pType) const
{
  return m_circle->centerInSceneCoords();
}

void FixedPort::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
  if ( event->button() == Qt::RightButton )
  {
    event->accept();

    if ( QMenu *menu = graph()->getFixedPortContextMenu( this ) )
    {
      menu->exec( QCursor::pos() );
      menu->deleteLater();
    }

    return;
  }
  
  ConnectionTarget::mousePressEvent( event );  
}

std::string FixedPort::path() const
{
  return m_name;
}

QString const FixedPort::MimeType( "x-fabric-ui/graph-view-fixed-port" );

bool FixedPort::MimeData::hasFormat( QString const &mimeType) const
{
  if ( mimeType == MimeType )
    return true;
  else return Parent::hasFormat( mimeType );
}

QStringList FixedPort::MimeData::formats() const
{
  QStringList result = Parent::formats();
  result.append( MimeType );
  return result;
}

QVariant FixedPort::MimeData::retrieveData(
  QString const &mimeType,
  QVariant::Type type
  ) const
{
  if ( mimeType == MimeType )
    return QVariant::fromValue( static_cast<void *>( m_port ) );
  else return Parent::retrieveData( mimeType, type );
}
