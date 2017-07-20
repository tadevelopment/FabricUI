//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_FCURVEITEM_H
#define FABRICUI_FCURVEEDITOR_FCURVEITEM_H

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <QGraphicsWidget>
#include <FTL/Config.h>
#include <set>

namespace FabricUI
{
namespace FCurveEditor
{

class FCurveItem : public QGraphicsWidget
{
  Q_OBJECT

public:
  enum KeyProp { POSITION, TAN_IN, TAN_OUT, NOTHING };
  enum Mode { SELECT, ADD, REMOVE, MODE_COUNT };

private:
  AbstractFCurveModel* m_curve;
  class FCurveShape;
  FCurveShape* m_curveShape;
  class KeyWidget;
  std::vector<KeyWidget*> m_keys;
  std::set<size_t> m_selectedKeys;
  KeyProp m_editedKeyProp;
  Mode m_currentMode;

  void addKey( size_t );
  void addKeyToSelection( size_t );
  void removeKeyFromSelection( size_t );
  void moveSelectedKeys( QPointF delta );
  void editKey( size_t, KeyProp p = POSITION );

public:
  FCurveItem();
  void setCurve( AbstractFCurveModel* );
  void clearKeySelection();
  void rectangleSelect( const QRectF&, Qt::KeyboardModifiers );
  void deleteSelectedKeys();
  inline KeyProp editedKeyProp() const { return m_editedKeyProp; }
  inline const std::set<size_t>& selectedKeys() const { return m_selectedKeys; }
  QRectF keysBoundingRect() const;
  void paint( QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) FTL_OVERRIDE;
  QRectF selectedKeysBoundingRect() const;

signals:
  void interactionBegin();
  void interactionEnd();
  void selectionChanged();
  void editedKeyValueChanged() const;
  void editedKeyPropChanged() const;

  // this signal is used to force a repaint after a mouse event (in order to
  // get proper feedback from it). Because if the eventKeyr takes more than ~40ms to compute
  // then the widget won't be repainted until the event stream stops.
  // HACK/TODO: use a native QGraphics method to achieve the same (and only repaint the
  // view where the mouse events come from, not the other views)
  void repaintViews() const;

private slots:
  void onKeyAdded();
  void onKeyDeleted( size_t );
  void onKeyMoved( size_t );
  inline void onDirty() { this->update(); }
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEITEM_H
