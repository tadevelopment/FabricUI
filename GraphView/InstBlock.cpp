//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/InstBlockHeader.h>

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QPainter>

namespace FabricUI {
namespace GraphView {

InstBlock::InstBlock(
  Node *node,
  QString name
  )
  : m_node( node )
{
  Graph const *graph = node->graph();
  GraphConfig const &config = graph->config();

  QGraphicsLinearLayout *layout = new QGraphicsLinearLayout();
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 1 );
  layout->setOrientation( Qt::Vertical );

  m_instBlockHeader = new InstBlockHeader( this, name );
  layout->addItem( m_instBlockHeader );
  layout->setAlignment( m_instBlockHeader, Qt::AlignHCenter | Qt::AlignVCenter );

  setContentsMargins( 8, 1, 8, 1 );
  setMinimumWidth( config.instBlockMinWidth );
  setMinimumHeight( config.instBlockMinHeight );
  setSizePolicy( QSizePolicy(
    QSizePolicy::MinimumExpanding,
    QSizePolicy::MinimumExpanding
    ) );
  setLayout( layout );


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
  rect.adjust( 6, 0, -6, 0 );

  painter->drawRect( rect );

  QGraphicsWidget::paint(painter, option, widget);
}

} // namespace GraphView
} // namespace FabricUI
