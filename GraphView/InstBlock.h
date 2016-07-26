//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef __UI_GraphView_InstBlock__
#define __UI_GraphView_InstBlock__

#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>
#include <QtWidgets/QGraphicsWidget>
#include <FabricUI/GraphView/GraphicItemTypes.h>

class QGraphicsLinearLayout;

namespace FabricUI {
namespace GraphView {

class InstBlockHeader;
class InstBlockPort;
class Node;

class InstBlock : public QGraphicsWidget
{
  Q_OBJECT

  friend class Node;

  typedef std::vector<InstBlockPort *> InstBlockPortVec;

public:

  InstBlock(
    Node *node,
    FTL::StrRef name
    );

  virtual int type() const { return QGraphicsItemType_InstBlock; }

  FTL::CStrRef name() const
    { return m_name; }
  QString name_QS() const
    { return QString::fromUtf8( m_name.data(), m_name.size() ); }
  void setName( FTL::StrRef newName );

  Node *node()
    { return m_node; }
  Node const *node() const
    { return m_node; }

  std::string path() const;

  size_t instBlockPortCount() const
    { return m_instBlockPorts.size(); }
  InstBlockPort *instBlockPort( size_t index ) const
    { return m_instBlockPorts[index]; }

  InstBlockPort *instBlockPort( FTL::StrRef name );

  InstBlockHeader *header()
    { return m_instBlockHeader; }

  void insertInstBlockPortAtIndex(
    unsigned index,
    InstBlockPort *instBlockPort
    );
  void reorderInstBlockPorts(
    FTL::ArrayRef<unsigned> newOrder
    );
  void removeInstBlockPortAtIndex(
    unsigned index
    );

  void updateLayout();
  void onConnectionsChanged()
    { updateLayout(); }

  bool isHighlighted() const
    { return m_isHighlighted; }

  void setFontColor( QColor col );

protected:

  virtual void paint(
    QPainter *painter,
    QStyleOptionGraphicsItem const *option,
    QWidget *widget
    ) /*override*/;
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

private:

  Node *m_node;
  std::string m_name;
  InstBlockHeader *m_instBlockHeader;
  InstBlockPortVec m_instBlockPorts;
  QGraphicsLinearLayout *m_layout;
  qreal m_pinRadius;
  bool m_isHighlighted;
};

} // namespace GraphView
} // namespace FabricUI

#endif // __UI_GraphView_InstBlock__
