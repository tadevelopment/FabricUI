//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/InstBlockHeader.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/NodeLabel.h>

#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsLinearLayout>
#include <QtGui/QPainter>

namespace FabricUI {
namespace GraphView {

InstBlock::InstBlock(
  Node *node,
  FTL::StrRef name
  )
  : m_node( node )
  , m_name( name )
  , m_isHighlighted( false )
{
  Graph const *graph = node->graph();
  GraphConfig const &config = graph->config();

  m_pinRadius = config.pinRadius;

  m_layout = new QGraphicsLinearLayout();
  m_layout->setContentsMargins( 0, 0, 0, 0 );
  m_layout->setSpacing( 5 );
  m_layout->setOrientation( Qt::Vertical );

  m_instBlockHeader = new InstBlockHeader( this, name );
  m_layout->addItem( m_instBlockHeader );
  m_layout->setAlignment( m_instBlockHeader, Qt::AlignHCenter | Qt::AlignVCenter );

  setContentsMargins( 0, 0, 0, 1 );
  setMinimumWidth( config.instBlockMinWidth );
  setMinimumHeight( config.instBlockMinHeight );
  setSizePolicy( QSizePolicy(
    QSizePolicy::MinimumExpanding,
    QSizePolicy::MinimumExpanding
    ) );
  setLayout( m_layout );

  setAcceptHoverEvents(true);
}

std::string InstBlock::path() const
{
  std::string result = m_node->name();
  result += '.';
  result += m_name;
  return result;
}

void InstBlock::insertInstBlockPortAtIndex(
  unsigned index,
  InstBlockPort *instBlockPort
  )
{
  instBlockPort->setFontColor( node()->fontColor() );

  m_instBlockPorts.insert( m_instBlockPorts.begin() + index, instBlockPort );
  m_layout->insertItem( 1 + index, instBlockPort );

  updateLayout();
}

void InstBlock::reorderInstBlockPorts(
  FTL::ArrayRef<unsigned> newOrder
  )
{
  assert( newOrder.size() == m_instBlockPorts.size() );
  InstBlockPortVec newInstBlockPorts;
  newInstBlockPorts.resize( newOrder.size() );
  for ( unsigned i = 0; i < m_instBlockPorts.size(); ++i )
    newInstBlockPorts[i] = m_instBlockPorts[newOrder[i]];
  newInstBlockPorts.swap( m_instBlockPorts );

  updateLayout();
}

void InstBlock::removeInstBlockPortAtIndex(
  unsigned index
  )
{
  InstBlockPort *instBlockPort = m_instBlockPorts[index];
  node()->graph()->removeConnectionsForConnectionTarget( instBlockPort ); 
  m_layout->removeAt( 1 + index );
  m_instBlockPorts.erase( m_instBlockPorts.begin() + index );
  instBlockPort->deleteLater();
  updateLayout();
}

void InstBlock::paint(
  QPainter *painter,
  QStyleOptionGraphicsItem const *option,
  QWidget *widget
  )
{
  QPen headerPen;
  if ( m_node->selected() )
    headerPen = m_node->selectedPen();
  else
    headerPen = m_node->defaultPen();

  QRectF rect = boundingRect();
  rect.adjust( m_pinRadius, 0, -m_pinRadius, 0 );

  QRectF headerRect = rect;
  qreal headerHeight = m_instBlockHeader->boundingRect().height();
  headerRect.setBottom( headerHeight );

  QRectF portsRect = rect;
  portsRect.setTop( headerRect.bottom() );
  portsRect.adjust( headerPen.width() * 0.5f, 0, -headerPen.width() * 0.5f, 0 );

  QBrush portsBrush;
  if ( m_isHighlighted )
    portsBrush = QBrush( m_node->color().lighter(110) );
  else
    portsBrush = QBrush( m_node->color() );
  painter->fillRect( portsRect, portsBrush );

  painter->setPen( headerPen );
  if ( m_isHighlighted )
    painter->setBrush( m_node->titleColor().lighter(120) );
  else
    painter->setBrush( m_node->titleColor() );
  painter->drawRect( headerRect );

  QGraphicsWidget::paint(painter, option, widget);
}

InstBlockPort *InstBlock::instBlockPort( FTL::StrRef name )
{
  for ( size_t index = 0; index < m_instBlockPorts.size(); ++index )
    if ( name == m_instBlockPorts[index]->name() )
      return m_instBlockPorts[index];
  return NULL;
}

void InstBlock::setName( FTL::StrRef newName )
{
  m_name = newName;
  m_instBlockHeader->setName(
    QString::fromUtf8( newName.data(), newName.size() )
    );
}

void InstBlock::updateLayout()
{
  prepareGeometryChange();

  for ( int i = m_layout->count(); --i; )
    m_layout->removeAt( i );

  Node::CollapseState nodeCollapseState = m_node->collapsedState();

  for ( size_t i = 0; i < m_instBlockPorts.size(); ++i )
  {
    InstBlockPort *instBlockPort = m_instBlockPorts[i];

    bool showPin =
      nodeCollapseState == Node::CollapseState_Expanded
      || ( nodeCollapseState == Node::CollapseState_OnlyConnections
        && instBlockPort->isConnected() );

    instBlockPort->setDrawState( showPin );
    if ( showPin )
    {
      m_layout->addItem( instBlockPort );
      m_layout->setAlignment(
        instBlockPort, Qt::AlignLeft | Qt::AlignTop
        );
    }

    instBlockPort->setDaisyChainCircleVisible(
      instBlockPort->isConnectedAsSource()
      );
  }
}

void InstBlock::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  node()->updateHighlightingFromChild( this, event->pos() );
}

void InstBlock::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  node()->updateHighlightingFromChild( this, event->pos() );
}

void InstBlock::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  node()->updateHighlightingFromChild( this, event->pos() );
}

void InstBlock::setFontColor( QColor color )
{
  m_instBlockHeader->setFontColor( color );

  for ( size_t i = 0; i < m_instBlockPorts.size(); ++i )
  {
    InstBlockPort *instBlockPort = m_instBlockPorts[i];
    instBlockPort->setFontColor( color );
  }
}

} // namespace GraphView
} // namespace FabricUI
