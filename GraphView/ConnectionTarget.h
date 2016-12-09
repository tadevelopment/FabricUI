// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_ConnectionTarget__
#define __UI_GraphView_ConnectionTarget__

#include <FTL/Config.h>
#include <QGraphicsWidget>
#include <QPointF>
#include "PortType.h"
#include <string>

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Graph;
    class PinCircle;

    enum TargetType
    {
      TargetType_Pin,
      TargetType_Port,
      TargetType_FixedPort,
      TargetType_ProxyPort,
      TargetType_MouseGrabber,
      TargetType_NodeHeader,
      TargetType_InstBlockHeader,
      TargetType_InstBlockPort
    };

    class ConnectionTarget : public QGraphicsWidget
    {

    public:

      ConnectionTarget(QGraphicsWidget * parent);
      ~ConnectionTarget();

      virtual std::string path() const = 0;
      QString path_QS() const
      {
        std::string p = path();
        return QString::fromUtf8( p.data(), p.size() );
      }

      virtual bool canConnectTo(
        ConnectionTarget * other,
        std::string &failureReason
        ) const = 0;

      virtual TargetType targetType() const = 0;
      virtual bool isRealPort() const { return false; }
      virtual QPointF connectionPos(PortType pType) const = 0;
    
      virtual Graph * graph() = 0;
      virtual const Graph * graph() const = 0;
      virtual QColor color() const = 0;

      virtual bool highlighted() const = 0;
      virtual void setHighlighted(bool state = true) = 0;

      virtual bool isConnected() const;
      virtual bool isConnectedAsSource() const;
      virtual bool isConnectedAsTarget() const;

      virtual bool selected() const;

      void hoverEnterEvent(QGraphicsSceneHoverEvent * event) FTL_OVERRIDE;
      void hoverMoveEvent( QGraphicsSceneHoverEvent * event ) FTL_OVERRIDE;
      void hoverLeaveEvent(QGraphicsSceneHoverEvent * event) FTL_OVERRIDE;
      void mousePressEvent(QGraphicsSceneMouseEvent * event) FTL_OVERRIDE;

    protected:
      virtual PinCircle * findPinCircle( QPointF pos ) { return NULL; }

    private:
      PinCircle * m_lastPinCircle;
#if defined(FTL_BUILD_DEBUG)
      bool m_deleted;
#endif
    };

  };

};

#endif // __UI_GraphView_ConnectionTarget__
