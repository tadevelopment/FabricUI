// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_InstBlockHeader__
#define __UI_GraphView_InstBlockHeader__

#include <FabricUI/GraphView/ConnectionTarget.h>

#include <FTL/StrRef.h>
#include <QString>
#include <QColor>

namespace FabricUI {
namespace GraphView {

class InstBlock;
class NodeLabel;
class PinCircle;

class InstBlockHeader : public ConnectionTarget
{
  Q_OBJECT

public:

  InstBlockHeader(
    InstBlock *instBlock,
    FTL::StrRef name
    );

  InstBlock *instBlock()
    { return m_instBlock; }
  InstBlock const *instBlock() const
    { return m_instBlock; }

  NodeLabel *nodeLabel()
    { return m_nodeLabel; }

  PinCircle const *inCircle() const
    { return m_inCircle; }
  PinCircle const *outCircle() const
    { return m_outCircle; }

  void setName( QString name );

  void setColor( QColor color );

  void setFontColor( QColor color );

  void setCirclesVisible(bool visible);

  // ConnectionTarget

  virtual std::string path() const /*override*/;

  virtual bool canConnectTo(
    ConnectionTarget *other,
    std::string &failureReason
    ) const /*override*/;

  virtual TargetType targetType() const /*override*/
    { return TargetType_InstBlockHeader; }

  virtual QPointF connectionPos(PortType pType) const /*override*/;

  virtual Graph *graph() /*override*/;
  virtual Graph const *graph() const /*override*/;
  virtual QColor color() const /*override*/;

  virtual bool highlighted() const /*override*/;
  virtual void setHighlighted( bool state = true ) /*override*/;

  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

private:

  InstBlock *m_instBlock;
  NodeLabel *m_nodeLabel;
  bool m_nodeButtonsHighlighted;
  PinCircle *m_inCircle;
  PinCircle *m_outCircle;
};

} // namespace GraphView
} // namespace FabricUI

#endif // __UI_GraphView_InstBlockHeader__
