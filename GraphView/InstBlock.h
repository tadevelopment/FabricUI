//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef __UI_GraphView_InstBlock__
#define __UI_GraphView_InstBlock__

#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>
#include <QtGui/QGraphicsWidget>

class QGraphicsLinearLayout;

namespace FabricUI {
namespace GraphView {

class InstBlockHeader;
class InstBlockPort;
class Node;

class InstBlock : public QGraphicsWidget
{
  Q_OBJECT

  typedef std::vector<InstBlockPort *> InstBlockPortVec;

public:

  InstBlock(
    Node *node,
    FTL::StrRef name
    );

  FTL::CStrRef name() const
    { return m_name; }
  QString name_QS() const
    { return QString::fromUtf8( m_name.data(), m_name.size() ); }

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

signals:

  void doubleClicked(
    FabricUI::GraphView::InstBlock *,
    Qt::MouseButton,
    Qt::KeyboardModifiers
    );

protected:

  virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );
  virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );
  virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
  virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

  virtual void paint(
    QPainter *painter,
    QStyleOptionGraphicsItem const *option,
    QWidget *widget
    ) /*override*/;

private:

  Node *m_node;
  std::string m_name;
  InstBlockHeader *m_instBlockHeader;
  InstBlockPortVec m_instBlockPorts;
  QGraphicsLinearLayout *m_layout;
  qreal m_pinRadius;
};

} // namespace GraphView
} // namespace FabricUI

#endif // __UI_GraphView_InstBlock__
