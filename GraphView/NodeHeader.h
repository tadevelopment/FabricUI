// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeHeader__
#define __UI_GraphView_NodeHeader__

#include <QGraphicsWidget>
#include <QColor>
#include <QPen>
#include "ConnectionTarget.h"
#include "PinCircle.h"
#include "NodeHeaderButton.h"
#include <vector>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Node;
    class NodeLabel;

    class NodeHeader : public ConnectionTarget
    {
      Q_OBJECT

    public:

      NodeHeader(
        Node * parent,
        QString const &title
        );

      Node * node();
      const Node * node() const;
      NodeLabel * labelWidget() { return m_title; }

      PinCircle const *inCircle() const
        { return m_inCircle; }
      PinCircle const *outCircle() const
        { return m_outCircle; }

      void setTitle( QString const &title, QString const& suffix );

      void setEditable( bool canEdit );

      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);

      virtual std::string path() const;

      // ConnectionTarget
      virtual bool canConnectTo(
        ConnectionTarget * other,
        std::string &failureReason
        ) const;

      virtual TargetType targetType() const { return TargetType_NodeHeader; }
      virtual QPointF connectionPos(PortType pType) const;
    
      virtual Graph * graph();
      virtual const Graph * graph() const;
      virtual QColor color() const;
      void setColor(QColor col);

      bool areCirclesVisible() const;
      void setCirclesVisible(bool visible);

      void setHeaderButtonState(QString name, int state);

      virtual bool nodeButtonsHighlighted() const { return m_nodeButtonsHighlighted; }
      virtual void setNodeButtonsHighlighted(bool value);

      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

    signals:

      void headerButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button);

    protected:

      PinCircle * findPinCircle( QPointF pos ) FTL_OVERRIDE { return NULL; }

    private slots:

      void onHeaderButtonTriggered(FabricUI::GraphView::NodeHeaderButton * button);

    private:

      void addHeaderButton(QString name, QStringList icons, int state = 0);

      Node * m_node;
      NodeLabel * m_title;
      bool m_nodeButtonsHighlighted;
      PinCircle * m_inCircle;
      PinCircle * m_outCircle;
      std::vector<NodeHeaderButton*> m_buttons;
    };

  };

};

#endif // __UI_GraphView_NodeHeader__
