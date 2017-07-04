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

  AbstractFCurveModel* m_model;
  QGraphicsScene* m_scene;
  FCurveItem* m_curveItem;
  class ValueEditor;
  ValueEditor* m_valueEditor;

public:
  FCurveEditor();
  ~FCurveEditor();
  void setModel( AbstractFCurveModel* );

protected:
  void mousePressEvent( QMouseEvent * ) FTL_OVERRIDE;
  void resizeEvent( QResizeEvent * ) FTL_OVERRIDE;

private slots:
  void onRectangleSelectReleased( const QRectF& );
  void onDeleteSelectedHandles();

signals:
  void interactionBegin();
  void interactionEnd();
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H
