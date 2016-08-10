// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/BackDropNode.h>
#include <FabricUI/GraphView/BlockRectangle.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/HighlightEffect.h>
#include <FabricUI/GraphView/InstBlock.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <FabricUI/GraphView/NodeLabel.h>
#include <FabricUI/GraphView/NodeRectangle.h>

#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QGraphicsDropShadowEffect>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>

#include <algorithm>

using namespace FabricUI::GraphView;

Node::Node(
  Graph * parent,
  NodeType nodeType,
  FTL::CStrRef name,
  FTL::CStrRef title,
  QColor color,
  QColor titleColor
  )
  : QGraphicsWidget( parent->itemGroup() )
  , m_graph( parent )
  , m_nodeType( nodeType )
  , m_name( name )
  , m_title( title )
  , m_bubble( NULL )
  , m_header( NULL )
  , m_mainWidget( NULL )
  , m_canAddPorts( false )
  , m_isHighlighted( false )
{
  m_defaultPen = m_graph->config().nodeDefaultPen;
  m_selectedPen = m_graph->config().nodeSelectedPen;
  m_errorPen = m_graph->config().nodeErrorPen;
  m_cornerRadius = m_graph->config().nodeCornerRadius;
  m_pinRadius = m_graph->config().pinRadius;
  m_collapsedState = CollapseState_Expanded;
  m_col = 0;
  m_alwaysShowDaisyChainPorts = false;

  if(color.isValid())
    setColor(color);
  else
    setColor(m_graph->config().nodeDefaultColor);
  if(titleColor.isValid())
    setTitleColor(titleColor);
  else
    setTitleColor(m_graph->config().nodeDefaultLabelColor);

  float contentMargins = m_graph->config().nodeContentMargins;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  if ( m_nodeType == NodeType_Block )
    m_mainWidget = new BlockRectangle( this );
  else
    m_mainWidget = new NodeRectangle( this );
  m_mainWidget->setMinimumWidth(m_graph->config().nodeMinWidth);
  m_mainWidget->setMinimumHeight(m_graph->config().nodeMinHeight);
  m_mainWidget->setSizePolicy(
    QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding)
    );

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  layout->addItem(m_mainWidget);
  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  layout = new QGraphicsLinearLayout();
  m_mainWidget->setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(contentMargins, 0, contentMargins, 0);
  layout->setSpacing(1);
  layout->setOrientation(Qt::Vertical);
  m_mainWidget->setLayout(layout);

  m_header = new NodeHeader(this, QSTRING_FROM_FTL_UTF8(title) );
  layout->addItem(m_header);
  layout->setAlignment(m_header, Qt::AlignHCenter | Qt::AlignVCenter);

  if ( !isBackDropNode() )
  {
    QObject::connect(m_header, SIGNAL(headerButtonTriggered(FabricUI::GraphView::NodeHeaderButton*)), 
      m_graph->controller(), SLOT(onNodeHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton*)));
  }

  m_pinsWidget = new QGraphicsWidget(m_mainWidget);
  layout->addItem(m_pinsWidget);
  layout->setAlignment(m_pinsWidget, Qt::AlignHCenter | Qt::AlignVCenter);

  m_pinsLayout = new QGraphicsLinearLayout();
  m_pinsLayout->setOrientation(Qt::Vertical);
  m_pinsLayout->setContentsMargins(0, m_graph->config().nodeSpaceAbovePorts, 0, m_graph->config().nodeSpaceBelowPorts);
  m_pinsLayout->setSpacing(m_graph->config().nodePinSpacing);
  m_pinsWidget->setLayout(m_pinsLayout);

  m_selected = false;
  m_dragging = 0;

  m_bubble = new GraphView::NodeBubble( this, graph()->config() );
  m_bubble->hide();

  setAcceptHoverEvents(true);

  updateEffect();
}

Node::~Node()
{
  m_bubble->setParentItem( NULL );
  m_bubble->scene()->removeItem( m_bubble );
  delete m_bubble;
}

NodeHeader * Node::header()
{
  return m_header;
}

const NodeHeader * Node::header() const
{
  return m_header;
}

NodeBubble * Node::bubble()
{
  return m_bubble;
}

const NodeBubble * Node::bubble() const
{
  return m_bubble;
}

void Node::setTitle( FTL::CStrRef title )
{
  m_title = title;
  if(m_header)
  {
    std::string titleToSet = m_title + m_titleSuffix;
    m_header->setTitle( QSTRING_FROM_FTL_UTF8(titleToSet) );
    m_header->labelWidget()->setItalic(m_titleSuffix.length() > 0);
  }
}

void Node::setTitleSuffix( FTL::CStrRef titleSuffix )
{
  m_titleSuffix = titleSuffix;
  setTitle(m_title);
}

QColor Node::color() const
{
  return m_colorA;
}

void Node::setColor(QColor col)
{
  setColorAsGradient(col, col);
}

void Node::setColorAsGradient(QColor a, QColor b)
{
  m_colorA = a;
  m_colorB = b;
  if ( m_graph->config().nodeDefaultPenUsesNodeColor )
    m_defaultPen.setBrush(m_colorB.darker());
  if ( m_mainWidget )
    m_mainWidget->update();
}

void Node::setTitleColor(QColor col)
{
  m_titleColor = col;
  if(m_header)
    m_header->setColor(m_titleColor.lighter());
}

QColor Node::fontColor() const
{
  return m_fontColor;
}

void Node::setFontColor(QColor col)
{
  if(m_fontColor == col)
    return;

  m_fontColor = col;
  m_header->labelWidget()->setColor(m_fontColor, m_header->labelWidget()->highlightColor());

  for(size_t i=0;i<m_pins.size();i++)
  {
    m_pins[i]->labelWidget()->setColor(m_fontColor, m_pins[i]->labelWidget()->highlightColor());
  }

  for ( size_t i = 0; i < m_instBlocks.size(); ++i )
  {
    InstBlock *instBlock = m_instBlocks[i];
    instBlock->setFontColor( col );
  }
}

QString Node::comment() const
{
  if(m_bubble == NULL)
    return "";
  return m_bubble->text();
}

void Node::setCollapsedState(Node::CollapseState state)
{
  if(m_collapsedState == state)
    return;
  m_collapsedState = state;
  if(!m_graph->config().nodeHeaderAlwaysShowPins)
    m_header->setCirclesVisible(state != CollapseState_Expanded);
  m_header->setHeaderButtonState("node_collapse", (int)m_collapsedState);
  updatePinLayout();
}

void Node::setSelected(bool state, bool quiet)
{
  if(state == m_selected)
    return;
  m_selected = state;
  if(m_header)
  {
    m_header->setHighlighted(m_selected);
    m_header->setNodeButtonsHighlighted(m_selected);
  }
  if(!quiet)
  {
    emit selectionChanged(this, m_selected);
    if(m_selected)
      emit m_graph->nodeSelected(this);
    else
      emit m_graph->nodeDeselected(this);
  }
  updateEffect();
  update();
}

QString Node::error() const
{
  return m_errorText;
}

bool Node::hasError() const
{
  return m_errorText.length() > 0;
}

void Node::setError(QString text)
{
  m_errorText = text;
  setToolTip( text );
  updateEffect();
}

void Node::clearError()
{
  setError("");
}

void Node::setGraphPos( QPointF pos, bool quiet )
{
  setTopLeftGraphPos( centralPosToTopLeftPos(pos), quiet );
}

void Node::setTopLeftGraphPos(QPointF pos, bool quiet)
{
  setPos( pos );
  if ( !quiet )
  {
    emit positionChanged( this, graphPos() );
    emit m_graph->nodeMoved( this, graphPos() );
  }
}

void Node::insertInstBlockAtIndex( unsigned index, InstBlock *instBlock )
{
  assert( std::find(
    m_instBlocks.begin(),
    m_instBlocks.end(),
    instBlock
    ) == m_instBlocks.end() );

  m_instBlocks.insert( m_instBlocks.begin() + index, instBlock );

  updatePinLayout();
}

InstBlock *Node::instBlockAtIndex( unsigned index ) const
{
  if ( index < m_instBlocks.size() )
    return m_instBlocks[index];
  else
    return 0;
}

void Node::renameInstBlockAtIndex( unsigned index, FTL::StrRef newName )
{
  m_instBlocks[index]->setName( newName );
}

void Node::removeInstBlockAtIndex( unsigned index )
{
  assert( index < m_instBlocks.size() );
  InstBlock *instBlock = m_instBlocks[index];

  for ( int i = int( instBlock->instBlockPortCount() ); i--; )
    instBlock->removeInstBlockPortAtIndex( i );

  m_instBlocks.erase( m_instBlocks.begin() + index );
  updatePinLayout();
  scene()->removeItem( instBlock );
  delete instBlock;

  // [pzion 20160216] Workaround for possible bug in QGraphicsScene
  // Without this, we get pretty consistent crashes walking the BSP
  // after pin removal.  After lots of debugging I believe this is 
  // a bug in QGraphicsScene, but I am not certain.
  scene()->setItemIndexMethod( QGraphicsScene::NoIndex );
  scene()->setItemIndexMethod( QGraphicsScene::BspTreeIndex );
}

bool Node::addPin( Pin *pin )
{
  // todo: we need a method to update the layout based on the collapsed state.....
  for(size_t i=0;i<m_pins.size();i++)
  {
    if(m_pins[i] == pin)
      return false;
    if(m_pins[i]->name() == pin->name())
      return false;
  }

  pin->setIndex((int)m_pins.size());
  m_pins.push_back(pin);

  updatePinLayout();

  return true;
}

bool Node::removePin( Pin * pin )
{
  size_t index = m_pins.size();
  for(size_t i=0;i<m_pins.size();i++)
  {
    if(m_pins[i] == pin)
    {
      index = i;
      break;
    }
  }
  if(index == m_pins.size())
    return false;

  graph()->removeConnectionsForConnectionTarget( pin );

  m_pins.erase(m_pins.begin() + index);
  updatePinLayout();

  scene()->removeItem(pin);
  
  delete pin;

  // [pzion 20160216] Workaround for possible bug in QGraphicsScene
  // Without this, we get pretty consistent crashes walking the BSP
  // after pin removal.  After lots of debugging I believe this is 
  // a bug in QGraphicsScene, but I am not certain.
  scene()->setItemIndexMethod( QGraphicsScene::NoIndex );
  scene()->setItemIndexMethod( QGraphicsScene::BspTreeIndex );

  return true;
}

void Node::reorderPins(QStringList names)
{
  std::vector<Pin*> pins;
  for(int i=0;i<names.length();i++)
  {
    pins.push_back(pin(names[i].toUtf8().constData()));
    pins[i]->setIndex(i);
  }
  m_pins = pins;
  updatePinLayout();
}

std::vector<Node*> Node::upStreamNodes(bool sortForPins, std::vector<Node*> rootNodes)
{
  int maxCol = 0;

  std::vector<GraphView::Node*> allNodes = graph()->nodes();
  for(unsigned int i=0;i<allNodes.size();i++)
  {
    allNodes[i]->setRow(-1);
    allNodes[i]->setCol(-1);
  }

  std::map<Node*, Node*> visitedNodes;
  std::vector<Node*> nodes;

  if(rootNodes.size() == 0)
  {
    visitedNodes.insert(std::pair<Node*, Node*>(this, this));
    nodes.push_back(this);
  }
  else
  {
    for(size_t i=0;i<rootNodes.size();i++)
    {
      visitedNodes.insert(std::pair<Node*, Node*>(rootNodes[i], rootNodes[i]));
      nodes.push_back(rootNodes[i]);
    }
  }

  for(size_t i=0;i<nodes.size();i++)
    nodes[i]->setCol(0);

  std::vector<Connection*> connections = graph()->connections();
  for(size_t i=0;i<nodes.size();i++)
  {
    for(size_t k=0;k<nodes[i]->m_pins.size();k++)
    {
      for(size_t j=0;j<connections.size();j++)
      {
        ConnectionTarget * dst = connections[j]->dst();
        ConnectionTarget * src = connections[j]->src();
        if(!dst || !src)
          continue;
        if(dst->targetType() != TargetType_Pin || src->targetType() != TargetType_Pin)
          continue;

        Pin * dstPin = (Pin*)dst;
        Pin * srcPin = (Pin*)src;
        Node * dstNode = dstPin->node();
        Node * srcNode = srcPin->node();

        if(dstNode != nodes[i])
          continue;
        if(visitedNodes.find(srcNode) != visitedNodes.end())
          continue;
        if(sortForPins && dstPin != nodes[i]->m_pins[k])
          continue;

        if(srcNode->col() < nodes[i]->col() + 1)
          srcNode->setCol(nodes[i]->col() + 1);
        if(srcNode->col() > maxCol)
          maxCol = srcNode->col();

        visitedNodes.insert(std::pair<Node*, Node*>(srcNode, srcNode));
        nodes.push_back(srcNode);
      }

      if(!sortForPins)
        break;
    }
  }

  std::vector<int> rows(maxCol+1);
  for(int i=0; i<maxCol; i++)
  {
    rows[i] = 0;
  }

  for(size_t i=0;i<nodes.size();i++)
  {
    nodes[i]->setRow(rows[nodes[i]->col()]);
    rows[nodes[i]->col()]++;
  }

  return nodes;
}

int Node::row() const
{
  return m_row;
}

void Node::setRow(int i)
{
  m_row = i;
}

int Node::col() const
{
  return m_col;
}

void Node::setCol(int i)
{
  m_col = i;
}

void Node::setAlwaysShowDaisyChainPorts(bool state)
{
  if(m_alwaysShowDaisyChainPorts == state)
    return;
  m_alwaysShowDaisyChainPorts = state;
  updatePinLayout(); 
}

unsigned int Node::pinCount() const
{
  return m_pins.size();
}

Pin * Node::pin(unsigned int index)
{
  return m_pins[index];
}

Pin * Node::pin(FTL::StrRef name)
{
  for(unsigned int i=0;i<m_pins.size();i++)
  {
    if(name == m_pins[i]->name())
      return m_pins[i];
  }
  return NULL;
}

Pin * Node::nextPin(FTL::StrRef name)
{
  for(unsigned int i=0;i<m_pins.size();i++)
  {
    if(name == m_pins[i]->name())
    {
      if (i + 1 < m_pins.size())
        return m_pins[i + 1];
      else
        return NULL;
    }
  }
  return NULL;
}

Pin *Node::renamePin( FTL::StrRef oldName, FTL::StrRef newName )
{
  Pin *p = pin( oldName );
  if ( p )
    p->setName( newName );
  return p;
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(onMousePress(event->button(), event->modifiers(), event->scenePos(), event->lastScenePos()))
  {
    event->accept();
    return;
  }
  QGraphicsWidget::mousePressEvent(event);
}

void Node::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
  if(onMouseMove(event->button(), event->modifiers(), event->scenePos(), event->lastScenePos()))
  {
    event->accept();
    return;
  }
  QGraphicsWidget::mouseMoveEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  if(onMouseRelease(event->button(), event->modifiers(), event->scenePos(), event->lastScenePos()))
  {
    event->accept();
    return;
  }
  QGraphicsWidget::mouseReleaseEvent(event);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
  if(onMouseDoubleClicked(event->button(), event->modifiers(), event->scenePos(), event->lastScenePos()))
  {
    event->accept();
    return;
  }
  QGraphicsWidget::mouseDoubleClickEvent(event);
}

bool Node::onMousePress(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos)
{
  if(modifiers.testFlag(Qt::AltModifier))
    return false;

  if ( button == Qt::LeftButton
    || button == Qt::MiddleButton
    || button == Qt::RightButton )
  {
    m_dragButton = button;
    m_mouseDownPos = scenePos;

    Node * hitNode = this;

    // apparently qt doesn't cast again on right
    // mouse button, so contextual menus are off.
    std::vector<Node *> nodes = graph()->nodes();
    for(size_t i=0;i<nodes.size();i++)
    {
      QPointF pos = nodes[i]->mapFromScene(scenePos);
      if(nodes[i]->rect().contains(pos))
      {
        if(nodes[i]->zValue() < hitNode->zValue())
          continue;
        hitNode = nodes[i];
      }
    }

    bool clearSelection = true;
    if(button == Qt::MiddleButton)
    {
      std::vector<Node*> nodes = hitNode->upStreamNodes();

      if(!modifiers.testFlag(Qt::ControlModifier) && !modifiers.testFlag(Qt::ShiftModifier))
      {
        m_graph->controller()->clearSelection();
        clearSelection = false;
      }

      for(size_t i=0;i<nodes.size();i++)
        m_graph->controller()->selectNode(nodes[i], true);
    }
    else if(button == Qt::RightButton)
    {
      clearSelection = !hitNode->selected();
    }

    m_dragging = button == Qt::RightButton ? 0 : 1;

    if(!hitNode->selected())
    {
      m_graph->controller()->beginInteraction();

      if(clearSelection && !modifiers.testFlag(Qt::ControlModifier) && !modifiers.testFlag(Qt::ShiftModifier))
        m_graph->controller()->clearSelection();
      m_graph->controller()->selectNode(hitNode, true);

      m_graph->controller()->endInteraction();
    }
    else if(modifiers.testFlag(Qt::ControlModifier))
    {
      m_graph->controller()->selectNode(hitNode, false);
    }

    m_nodesToMove = m_graph->selectedNodes();
    if ( !modifiers.testFlag( Qt::ShiftModifier ) )
    {
      std::vector<Node *> additionalNodes;

      for ( std::vector<Node *>::const_iterator it = m_nodesToMove.begin();
        it != m_nodesToMove.end(); ++it )
      {
        Node *node = *it;
        if ( node->isBackDropNode() )
        {
          BackDropNode *backDropNode = static_cast<BackDropNode *>( node );
          backDropNode->appendOverlappingNodes( additionalNodes );
        }
      }

      m_nodesToMove.insert(
        m_nodesToMove.end(),
        additionalNodes.begin(),
        additionalNodes.end()
        );
    }

    if(button == Qt::RightButton)
    {
      QMenu * menu = graph()->getNodeContextMenu(hitNode);
      if(menu)
      {
        menu->exec( QCursor::pos() );
        menu->setParent( NULL );
        menu->deleteLater();
      }
    }

    return true;
  }

  return false;
}

bool Node::onMouseMove(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos)
{
  if ( m_dragging > 0 )
  {
    m_dragging = 2;

    QPointF delta = scenePos - lastScenePos;
    delta *= 1.0f / graph()->mainPanel()->canvasZoom();

    m_graph->controller()->gvcDoMoveNodes(
      m_nodesToMove,
      delta,
      false // allowUndo
      );

    return true;
  }

  return false;
}

bool Node::onMouseRelease(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos)
{
  if ( m_dragging == 2 )
  {
    if(!selected())
      emit positionChanged(this, graphPos());
    else
    {
      QPointF delta;

      delta = m_mouseDownPos - lastScenePos;
      delta *= 1.0f / graph()->mainPanel()->canvasZoom();

      m_graph->controller()->gvcDoMoveNodes(
        m_nodesToMove,
        delta,
        false // allowUndo
        );

      delta = scenePos - m_mouseDownPos;
      delta *= 1.0f / graph()->mainPanel()->canvasZoom();

      m_graph->controller()->gvcDoMoveNodes(
        m_nodesToMove,
        delta,
        true // allowUndo
        );
    }

    m_dragging = 0;
    m_nodesToMove.clear();

    return true;
  }

  return false;
}

bool Node::onMouseDoubleClicked(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos)
{
  if(button == Qt::LeftButton)
  {
    emit doubleClicked(this, button, modifiers);
    return true;
  }
  return false;
}

void Node::updateHighlightingFromChild( QGraphicsItem *child, QPointF cp )
{
  updateHighlighting( mapFromItem( child, cp ) );
}

void Node::updateHighlighting( QPointF cp )
{
  QRectF br = QGraphicsWidget::boundingRect();

  // qDebug() << "Node::updateHighlighting cp:" << cp << " br:" << br;

  bool oldIsHighlighted = m_isHighlighted;

  if ( br.contains( cp ) )
  {
    bool someInstBlockHighlighted = false;
    for ( int i = int( m_instBlocks.size() ); i--; )
    {
      InstBlock *instBlock = m_instBlocks[i];
      bool oldIsHighlighted = instBlock->m_isHighlighted;
      QRectF instBlockBoundingRect(
        mapFromItem( instBlock, instBlock->boundingRect().topLeft() ),
        br.bottomRight()
        );
      // qDebug() << "i:" << i << " cp:" << cp << " br:" << instBlockBoundingRect;
      instBlock->m_isHighlighted =
          !someInstBlockHighlighted
        && instBlockBoundingRect.contains( cp );
      if ( instBlock->m_isHighlighted != oldIsHighlighted )
        instBlock->update();
      if ( instBlock->m_isHighlighted )
        someInstBlockHighlighted = true;
    }

    if ( !someInstBlockHighlighted
      && !m_canAddPorts
      && !m_instBlocks.empty() )
    {
      InstBlock *instBlock = m_instBlocks[0];
      bool oldIsHighlighted = instBlock->m_isHighlighted;
      instBlock->m_isHighlighted = true;
      if ( instBlock->m_isHighlighted != oldIsHighlighted )
        instBlock->update();

      someInstBlockHighlighted = true;
    }

    m_isHighlighted =
        m_nodeType == NodeType_Inst
      // && ( QApplication::keyboardModifiers() & Qt::ShiftModifier )
      && !someInstBlockHighlighted;
  }
  else
  {
    m_isHighlighted = false;
    for ( size_t i = 0; i < m_instBlocks.size(); ++i )
    {
      InstBlock *instBlock = m_instBlocks[i];
      bool oldIsHighlighted = instBlock->m_isHighlighted;
      instBlock->m_isHighlighted = false;
      if ( instBlock->m_isHighlighted != oldIsHighlighted )
        instBlock->update();
    }
  }

  if ( m_isHighlighted != oldIsHighlighted )
    m_mainWidget->update();
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  updateHighlighting( event->pos() );
}

void Node::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  updateHighlighting( event->pos() );
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  updateHighlighting( event->pos() );
}

QGraphicsWidget * Node::mainWidget()
{
  return m_mainWidget;
}

QGraphicsWidget * Node::pinsWidget()
{
  return m_pinsWidget;
}

void Node::onConnectionsChanged()
{
  if(m_collapsedState == CollapseState_OnlyConnections)
  {
    updatePinLayout();
  }
}

void Node::onBubbleEditRequested(FabricUI::GraphView::NodeBubble * bubble)
{
  emit bubbleEditRequested(this);  
}

void Node::updatePinLayout()
{
  prepareGeometryChange();

  int count = m_pinsLayout->count();
  if(count > 0)
  {
    for(int i=count-1;i>=0;i--)
      m_pinsLayout->removeAt(i);
    m_pinsLayout->invalidate();
  }

  for(size_t i=0;i<m_pins.size();i++)
  {
    bool showPin =
      ( !isInstNode() || i > 0 ) // don't show exec port ever
      && ( m_collapsedState == CollapseState_Expanded
        || ( m_collapsedState == CollapseState_OnlyConnections
          && m_pins[i]->isConnected() ) );

    m_pins[i]->setDrawState(showPin);
    if(showPin)
    {
      m_pinsLayout->addItem(m_pins[i]);
      m_pinsLayout->setAlignment(m_pins[i], Qt::AlignLeft | Qt::AlignTop);
    }
  }

  for ( size_t i = 0; i < m_instBlocks.size(); ++i )
  {
    InstBlock *instBlock = m_instBlocks[i];
    instBlock->updateLayout();
    m_pinsLayout->addItem( instBlock );
    m_pinsLayout->setAlignment( instBlock, Qt::AlignLeft | Qt::AlignTop );
  }

  if(m_pinsLayout->count() == 0 && m_pins.size() > 0)
  {
    m_pinsLayout->addItem(m_pins[0]);
    m_pinsLayout->setAlignment(m_pins[0], Qt::AlignLeft | Qt::AlignTop);
  }

  for(size_t i=0;i<m_pins.size();i++)
  {
    m_pins[i]->setDaisyChainCircleVisible(
      m_alwaysShowDaisyChainPorts || m_pins[i]->isConnectedAsSource()
      );
  }
}

void Node::updateEffect()
{
  prepareGeometryChange();
}

QRectF Node::boundingRect() const
{
  return QGraphicsWidget::boundingRect().adjusted( 0, -4, 0, 4 );
}

void Node::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  if ( !m_errorText.isEmpty() )
  {
    QRectF rect = QRectF(
      QPointF( 0, 0 ),
      m_mainWidget->size()
      );
    rect.adjust( 7, 0, -7, 0 ); // compensate for ports

    painter->setPen( Qt::NoPen );
    painter->setBrush( QColor( 255, 0, 0, 128 ) );
    rect.adjust( -4, -4, 4, 4 );
    painter->drawRoundedRect( rect, 6, 6 );
  }
  // [pzion 20160225] Disable shadow for now
  // else
  // {
  //   QRectF rect = QRectF(
  //     QPointF( 0, 0 ),
  //     m_mainWidget->size()
  //     );
  //   rect.adjust( 7, 0, -7, 0 ); // compensate for ports

  //   painter->setPen( Qt::NoPen );
  //   painter->setBrush( QColor( 0, 0, 0, 128 ) );
  //   rect.adjust( 4, 4, 4, 4 );
  //   painter->drawRoundedRect( rect, 4, 4 );
  // }

  QGraphicsWidget::paint(painter, option, widget);
}

InstBlock *Node::instBlock( FTL::StrRef name )
{
  for ( size_t index = 0; index < m_instBlocks.size(); ++index )
    if ( name == m_instBlocks[index]->name() )
      return m_instBlocks[index];
  return NULL;
}

void Node::collectEditingTargets( EditingTargets &editingTargets )
{
  editingTargets.reserve( 1 + m_instBlocks.size() );

  if ( m_canAddPorts )
    editingTargets.push_back( EditingTarget( this, 0 ) );

  for ( size_t i = 0; i < m_instBlocks.size(); ++i )
    editingTargets.push_back( EditingTarget( m_instBlocks[i], 0 ) );

  if ( !m_canAddPorts )
    editingTargets.push_back( EditingTarget( this, 1 ) );
}

