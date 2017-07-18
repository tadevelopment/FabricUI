//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H
#define FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H

#include <QWidget>
#include <FabricUI/FCurveEditor/RuledGraphicsView.h>

class QGraphicsScene;

namespace FabricUI
{
namespace FCurveEditor
{
class AbstractFCurveModel;
class FCurveItem;

class FCurveEditor : public RuledGraphicsView
{
  Q_OBJECT

  typedef RuledGraphicsView Parent;

  // relative position of the value editor (negative values will be from the right/bottom)
  Q_PROPERTY( QPoint vePos READ vePos WRITE setVEPos )
  QPoint m_vePos;

  AbstractFCurveModel* m_model;
  QGraphicsScene* m_scene;
  FCurveItem* m_curveItem;
  class ValueEditor;
  ValueEditor* m_valueEditor;
  void veEditFinished( bool isXNotY );
  void updateVEPos();

public:
  FCurveEditor();
  ~FCurveEditor();
  void setModel( AbstractFCurveModel* );
  void frameAllKeys();
  void frameSelectedKeys();

  inline QPoint vePos() const { return m_vePos; }
  inline void setVEPos( const QPoint& p ) { m_vePos = p; this->updateVEPos(); }

protected:
  void mousePressEvent( QMouseEvent * ) FTL_OVERRIDE;
  void resizeEvent( QResizeEvent * ) FTL_OVERRIDE;

private slots:
  void onRectangleSelectReleased( const QRectF&, Qt::KeyboardModifiers );
  void onFrameAllKeys();
  void onFrameSelectedKeys();
  void onDeleteSelectedKeys();
  void onStartEditingKey();
  void onEditedKeyValueChanged();
  void onStopEditingKey();
  void onRepaintViews();
  inline void veXEditFinished() { this->veEditFinished( true ); }
  inline void veYEditFinished() { this->veEditFinished( false ); }

signals:
  void interactionBegin();
  void interactionEnd();
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H
