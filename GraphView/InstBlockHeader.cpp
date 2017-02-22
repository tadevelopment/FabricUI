// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/InstBlockHeader.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/NodeLabel.h>
#include <FabricUI/GraphView/Graph.h>

#include <QGraphicsLinearLayout>
#include <QPushButton>
#include <QGraphicsProxyWidget>

using namespace FabricUI::GraphView;

InstBlockHeader::InstBlockHeader(
  InstBlock *instBlock,
  FTL::StrRef name
  )
  : ConnectionTarget( instBlock )
  , m_instBlock( instBlock )
  , m_nodeButtonsHighlighted( false )
  , m_inCircle( 0 )
  , m_outCircle( 0 )
{
  Node *node = instBlock->node();
  Graph *graph = node->graph();
  GraphConfig const &graphConfig = graph->config();

  setSizePolicy( QSizePolicy(
    QSizePolicy::Expanding,
    QSizePolicy::Expanding
    ) );

  QGraphicsLinearLayout *layout = new QGraphicsLinearLayout();
  float contentsMargins = graphConfig.nodeHeaderContentMargins;

  setContentsMargins( 0, 0, 0, 0 );
  layout->setContentsMargins(0, contentsMargins + 1, 0, contentsMargins);
  layout->setSpacing(graphConfig.nodeHeaderSpacing);
  layout->setOrientation(Qt::Horizontal);
  setLayout(layout);

  m_nodeLabel = new NodeLabel(
    this,
    node,
    QString::fromUtf8( name.data(), name.size() ),
    graphConfig.nodeFontColor,
    graphConfig.nodeFontHighlightColor,
    graphConfig.instBlockHeaderFont
    );

  m_inCircle = new PinCircle( this, PortType_Input, node->color() );
  layout->addItem(m_inCircle);
  layout->setAlignment(m_inCircle, Qt::AlignLeft | Qt::AlignVCenter);

  layout->addStretch(1);

  layout->addItem(m_nodeLabel);
  layout->setAlignment(m_nodeLabel, Qt::AlignHCenter | Qt::AlignVCenter);

  layout->addStretch(1);

  m_outCircle = new PinCircle(
    this,
    PortType_Output,
    node->color(),
    true, // interactiveConnectionsAllowed
    true // invisible
    );
  layout->addItem(m_outCircle);
  layout->setAlignment(m_outCircle, Qt::AlignRight | Qt::AlignVCenter);
  m_outCircle->setEnabled( false );
}

void InstBlockHeader::setName( QString name )
{
  m_nodeLabel->setText( name );
}

Graph *InstBlockHeader::graph()
{
  return m_instBlock->node()->graph();
}

Graph const *InstBlockHeader::graph() const
{
  return m_instBlock->node()->graph();
}

QColor InstBlockHeader::color() const
{
  return m_instBlock->node()->color();
}

bool InstBlockHeader::highlighted() const
{
  return m_nodeLabel->highlighted();
}

void InstBlockHeader::setHighlighted( bool state )
{
  m_nodeLabel->setHighlighted( state );
}

std::string InstBlockHeader::path() const
{
  std::string result = m_instBlock->node()->name();
  result += '.';
  result += m_nodeLabel->text().toUtf8().constData();
  return result;
}

bool InstBlockHeader::canConnectTo(
  ConnectionTarget *other,
  std::string &failureReason
  ) const
{
  return false;
}

QPointF InstBlockHeader::connectionPos(PortType pType) const
{
  if(pType == PortType_Input)
  {
    if(m_inCircle)
      return m_inCircle->centerInSceneCoords();
  }
  else
  {
    if(m_outCircle)
      return m_outCircle->centerInSceneCoords();
  }
  return QPointF();
}

void InstBlockHeader::setColor( QColor color )
{
  if ( m_inCircle )
    m_inCircle->setColor( color );
  if ( m_outCircle )
    m_outCircle->setColor( color );
}

void InstBlockHeader::setCirclesVisible( bool visible )
{
  m_inCircle->setVisible( visible );
}

void InstBlockHeader::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  instBlock()->node()->updateHighlightingFromChild( this, event->pos() );
}

void InstBlockHeader::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  instBlock()->node()->updateHighlightingFromChild( this, event->pos() );
}

void InstBlockHeader::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  instBlock()->node()->updateHighlightingFromChild( this, event->pos() );
}

void InstBlockHeader::setFontColor( QColor color )
{
  m_nodeLabel->setColor( color, m_nodeLabel->highlightColor() );
}
