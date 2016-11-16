//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef _FabricUI_GraphView_InstBlockPort_h
#define _FabricUI_GraphView_InstBlockPort_h

#include <QGraphicsWidget>
#include <QColor>
#include <QPen>

#include <FTL/CStrRef.h>

#include "PortType.h"
#include "NodeLabel.h"
#include "PinCircle.h"
#include "ConnectionTarget.h"
#include "GraphicItemTypes.h"

namespace FabricUI {
namespace GraphView {

class Graph;
class InstBlock;
class Node;

class InstBlockPort : public ConnectionTarget
{
  Q_OBJECT

public:

  virtual ~InstBlockPort() {}

  virtual int type() const { return QGraphicsItemType_Pin; }

  InstBlock *instBlock()
    { return m_instBlock; }
  InstBlock const *instBlock() const
    { return m_instBlock; }

  Node *node();
  Node const *node() const;

  Graph * graph();
  const Graph * graph() const;

  FTL::CStrRef name() const
    { return m_name; }
  void setName( FTL::StrRef newName );

  virtual std::string path() const;

  virtual char const * label() const;
  TextContainer * labelWidget() { return m_label; }
  virtual PortType portType() const;
  QColor color() const;
  void setColor(QColor color, bool quiet = false, bool performUpdate = true);
  virtual int index() const;
  virtual void setIndex(int i);
  virtual bool highlighted() const;
  virtual void setHighlighted(bool state = true);
  virtual FTL::CStrRef dataType() const
    { return m_dataType; }
  virtual void setDataType(FTL::CStrRef dataType);

  PinCircle * inCircle();
  const PinCircle * inCircle() const;
  PinCircle * outCircle();
  const PinCircle * outCircle() const;

  virtual bool canConnectTo(
    ConnectionTarget * other,
    std::string &failureReason
    ) const;

  virtual TargetType targetType() const { return TargetType_InstBlockPort; }
  virtual bool isRealPort() const { return true; }
  virtual QPointF connectionPos(PortType pType) const;

  void setDrawState(bool flag);
  bool drawState() const
    { return m_drawState; }

  virtual void setDaisyChainCircleVisible(bool flag);

  virtual bool selected() const /*override*/;

  void setFontColor( QColor color );

  // accessed by controller
  InstBlockPort(
    InstBlock *instBlock,
    FTL::StrRef name,
    PortType pType,
    QColor color
    );
  
signals:

  void colorChanged( InstBlockPort *, QColor );
  void inCircleScenePositionChanged();
  void outCircleScenePositionChanged();
  void drawStateChanged();

private:

  InstBlock *m_instBlock;
  std::string m_name;
  PortType m_portType;
  std::string m_dataType;
  std::string m_labelCaption;
  std::string m_labelSuffix;
  bool m_highlighted;
  QColor m_color;
  int m_index;
  NodeLabel * m_label;
  PinCircle * m_inCircle;
  PinCircle * m_outCircle;
  bool m_drawState;
};

} // namespace GraphView
} // namespace FabricUI

#endif // _FabricUI_GraphView_InstBlockPort_h

