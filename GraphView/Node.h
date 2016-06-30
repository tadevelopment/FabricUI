// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Node__
#define __UI_GraphView_Node__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include <FTL/CStrRef.h>

#include <FabricUI/GraphView/NodeRectangle.h>
#include <FabricUI/GraphView/NodeHeader.h>
#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/GraphicItemTypes.h>

#include <vector>

namespace FabricUI
{

  namespace GraphView
  {

    // forward declarations
    class Graph;
    class InstBlock;
    class NodeBubble;

    class Node : public QGraphicsWidget
    {
      Q_OBJECT

      friend class Graph;
      friend class InstBlock;
      friend class BlockRectangle;
      friend class NodeRectangle;
      friend class NodeBubble;
      friend class NodeHeaderButton;

    public:

      typedef std::pair<QGraphicsWidget *, int> EditingTarget;
      typedef std::vector<EditingTarget> EditingTargets;

      enum NodeType
      {
        NodeType_Plain,
        NodeType_Inst,
        NodeType_BackDrop,
        NodeType_Block
      };

      enum CollapseState
      {
        CollapseState_Expanded,
        CollapseState_OnlyConnections,
        CollapseState_Collapsed,
        CollapseState_NumStates
      };

      Node(
        Graph * parent,
        NodeType nodeType,
        FTL::CStrRef name,
        FTL::CStrRef title,
        QColor color = QColor(),
        QColor titleColor = QColor()
        );
      virtual ~Node();

      virtual int type() const { return QGraphicsItemType_Node; }

      NodeType getNodeType() const
        { return m_nodeType; }
      bool isInstNode() const
        { return m_nodeType == NodeType_Inst; }
      bool isBackDropNode() const
        { return m_nodeType == NodeType_BackDrop; }
      bool isBlockNode() const
        { return m_nodeType == NodeType_Block; }

      Graph *graph()
        { return m_graph; }
      Graph const *graph() const
        { return m_graph; }

      NodeHeader * header();
      const NodeHeader * header() const;
      NodeBubble * bubble();
      const NodeBubble * bubble() const;

      FTL::CStrRef name() const
        { return m_name; }
      QString name_QS() const
        { return QString::fromUtf8( m_name.data(), m_name.size() ); }
        
      FTL::CStrRef title() const
        { return m_title; }
      void setTitle( FTL::CStrRef title );
      FTL::CStrRef titleSuffix() const
        { return m_titleSuffix; }
      void setTitleSuffix( FTL::CStrRef titleSuffix );
      void setTitleSuffixAsterisk() { setTitleSuffix(" *"); }
      void removeTitleSuffix() { setTitleSuffix(""); }
      QColor color() const;
      virtual void setColor(QColor col);
      virtual void setColorAsGradient(QColor a, QColor b);

      QColor titleColor() const
        { return m_titleColor; }
      virtual void setTitleColor(QColor col);

      QColor fontColor() const;
      virtual void setFontColor(QColor col);

      QPen defaultPen() const
        { return m_defaultPen; }
      QPen selectedPen() const
        { return m_selectedPen; }

      QString comment() const;

      bool selected() const
        { return m_selected; }

      CollapseState collapsedState() const
        { return m_collapsedState; }
      virtual void setCollapsedState(CollapseState state);
      virtual void toggleCollapsedState();

      virtual QString error() const;
      virtual bool hasError() const;
      virtual void setError(QString text);
      virtual void clearError();

      QPointF graphPos() const
        { return topLeftToCentralPos( topLeftGraphPos() ); }
      void setGraphPos( QPointF pos, bool quiet = false );

      QPointF topLeftToCentralPos( QPointF pos ) const
      {
        QRectF rect = boundingRect();
        return QPointF(
          pos.x() + rect.width() * 0.5,
          pos.y() + rect.height() * 0.5
          );
      }
      QPointF centralPosToTopLeftPos( QPointF pos ) const
      {
        QRectF rect = boundingRect();
        return QPointF(pos.x() - rect.width() * 0.5, pos.y() - rect.height() * 0.5);
      }

      QPointF topLeftGraphPos() const
        { return pos(); }
      void setTopLeftGraphPos( QPointF pos, bool quiet = false );

      virtual unsigned int pinCount() const;
      virtual Pin * pin(unsigned int index);
      virtual Pin * pin(FTL::StrRef name);

      virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

      virtual QRectF boundingRect() const;
      virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

      // accessed by controller
      virtual void setSelected(bool state, bool quiet = false);

      bool addPin( Pin * pin );
      bool removePin( Pin * pin );

      Pin *renamePin( FTL::StrRef oldName, FTL::StrRef newName );
      virtual void reorderPins(QStringList names);

      void insertInstBlockAtIndex( unsigned index, InstBlock *instBlock );
      InstBlock *instBlockAtIndex( unsigned index ) const;
      void renameInstBlockAtIndex( unsigned index, FTL::StrRef newName );
      void removeInstBlockAtIndex( unsigned index );

      virtual std::vector<Node*> upStreamNodes(bool sortForPins = false, std::vector<Node*> rootNodes = std::vector<Node*>());
      // temporary information around row and col
      virtual int row() const;
      virtual void setRow(int i);
      virtual int col() const;
      virtual void setCol(int i);

      virtual void setAlwaysShowDaisyChainPorts(bool state);

      InstBlock *instBlock( FTL::StrRef name );

      QGraphicsWidget * mainWidget();
      QGraphicsWidget * pinsWidget();

      bool canAddPorts() const
        { return m_canAddPorts; }
      void setCanAddPorts( bool canAddPorts )
        { m_canAddPorts = canAddPorts; }

      void collectEditingTargets( EditingTargets &editingTargets );

    public slots:

      void onConnectionsChanged();
      void onBubbleEditRequested(FabricUI::GraphView::NodeBubble * bubble);
    
    signals:

      void selectionChanged(FabricUI::GraphView::Node *, bool);
      void collapsedStateChanged(FabricUI::GraphView::Node *, FabricUI::GraphView::Node::CollapseState);
      void positionChanged(FabricUI::GraphView::Node *, QPointF);
      void doubleClicked(FabricUI::GraphView::Node *, Qt::MouseButton, Qt::KeyboardModifiers);
      void bubbleEditRequested(FabricUI::GraphView::Node * nod);
      void geometryChanged();

    protected:

      void updateEffect();
      void updatePinLayout();

      // used by NodeHeader / NodeHeaderButton
      bool onMousePress(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos);
      bool onMouseMove(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos);
      bool onMouseRelease(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos);
      bool onMouseDoubleClicked(Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QPointF scenePos, QPointF lastScenePos);

      Graph * m_graph;
      NodeType m_nodeType;
      std::string m_name;
      std::string m_title;
      std::string m_titleSuffix;
      NodeBubble * m_bubble;

      QColor m_colorA;
      QColor m_colorB;
      QColor m_titleColor;
      QColor m_fontColor;
      QPen m_defaultPen;
      QPen m_selectedPen;
      QPen m_errorPen;
      float m_cornerRadius;
      QString m_errorText;
      CollapseState m_collapsedState;

      NodeHeader * m_header;
      QGraphicsWidget *m_mainWidget;
      QGraphicsWidget * m_pinsWidget;
      QGraphicsLinearLayout * m_pinsLayout;
      bool m_selected;
      int m_dragging;
      Qt::MouseButton m_dragButton;
      QPointF m_mouseDownPos;
      std::vector<Node *> m_nodesToMove;

      std::vector<Pin*> m_pins;
      int m_row;
      int m_col;
      bool m_alwaysShowDaisyChainPorts;

      std::vector<InstBlock *> m_instBlocks;

      bool m_canAddPorts;
    };


  };

};

#endif // __UI_GraphView_Node__
