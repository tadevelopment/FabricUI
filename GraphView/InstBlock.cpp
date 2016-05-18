//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/InstBlockPort.h>
#include <FabricUI/GraphView/InstBlockHeader.h>

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QPainter>

namespace FabricUI {
namespace GraphView {

InstBlock::InstBlock(
  Node *node,
  FTL::StrRef name
  )
  : m_node( node )
  , m_name( name )
{
  Graph const *graph = node->graph();
  GraphConfig const &config = graph->config();

  m_pinRadius = config.pinRadius;

  m_layout = new QGraphicsLinearLayout();
  m_layout->setContentsMargins( 0, 0, 0, 4 );
  m_layout->setSpacing( 4 );
  m_layout->setOrientation( Qt::Vertical );

  m_instBlockHeader = new InstBlockHeader( this, name );
  m_layout->addItem( m_instBlockHeader );
  m_layout->setAlignment( m_instBlockHeader, Qt::AlignHCenter | Qt::AlignVCenter );

  setContentsMargins( 7, 1, 7, 1 );
  setMinimumWidth( config.instBlockMinWidth );
  setMinimumHeight( config.instBlockMinHeight );
  setSizePolicy( QSizePolicy(
    QSizePolicy::MinimumExpanding,
    QSizePolicy::MinimumExpanding
    ) );
  setLayout( m_layout );


  // if ( !isBackDropNode() )
  // {
  //   QObject::connect(m_header, SIGNAL(headerButtonTriggered(FabricUI::GraphView::NodeHeaderButton*)), 
  //     m_graph->controller(), SLOT(onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton*)));
  // }

  // m_pinsWidget = new QGraphicsWidget(rectangle);
  // layout->addItem(m_pinsWidget);
  // layout->setAlignment(m_pinsWidget, Qt::AlignHCenter | Qt::AlignVCenter);

  // m_pinsLayout = new QGraphicsLinearLayout();
  // m_pinsLayout->setOrientation(Qt::Vertical);
  // m_pinsLayout->setContentsMargins(0, m_graph->config().nodeSpaceAbovePorts, 0, m_graph->config().nodeSpaceBelowPorts);
  // m_pinsLayout->setSpacing(m_graph->config().nodePinSpacing);
  // m_pinsWidget->setLayout(m_pinsLayout);
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
  m_instBlockPorts.insert( m_instBlockPorts.begin() + index, instBlockPort );
  m_layout->insertItem( 1 + index, instBlockPort );
}

void InstBlock::removeInstBlockPortAtIndex(
  unsigned index
  )
{
  m_layout->removeAt( 1 + index );
  m_instBlockPorts.erase( m_instBlockPorts.begin() + index );
}

void InstBlock::paint(
  QPainter *painter,
  QStyleOptionGraphicsItem const *option,
  QWidget *widget
  )
{
  if ( m_node->selected() )
    painter->setPen( m_node->selectedPen() );
  else
    painter->setPen( m_node->defaultPen() );
  
  painter->setBrush( m_node->titleColor() );

  QRectF rect = contentsRect();
  rect.adjust( m_pinRadius, 0, -m_pinRadius, 0 );

  painter->drawRect( rect );

  qreal headerHeight = m_instBlockHeader->boundingRect().height();
  painter->drawLine(
    rect.topLeft() + QPointF( 0, headerHeight ),
    rect.topRight() + QPointF( 0, headerHeight )
    );

  QGraphicsWidget::paint(painter, option, widget);
}

} // namespace GraphView
} // namespace FabricUI
