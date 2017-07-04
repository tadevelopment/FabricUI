//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_FCURVEITEM_H
#define FABRICUI_FCURVEEDITOR_FCURVEITEM_H

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <QGraphicsWidget>
#include <set>

namespace FabricUI
{
namespace FCurveEditor
{

class FCurveItem : public QGraphicsWidget
{
  Q_OBJECT

  AbstractFCurveModel* m_curve;
  class FCurveShape;
  FCurveShape* m_curveShape;
  class HandleWidget;
  std::vector<HandleWidget*> m_handles;
  std::set<size_t> m_selectedHandles;

  void addHandle( size_t );
  void clearHandleSelection();
  void addHandleToSelection( size_t );
  void moveSelectedHandles( QPointF delta );

public:
  FCurveItem();
  void setCurve( AbstractFCurveModel* );
  void rectangleSelect( const QRectF& );
  void deleteSelectedHandles();

signals:
  void interactionBegin();
  void interactionEnd();

private slots:
  void onHandleAdded();
  void onHandleDeleted( size_t );
  void onHandleMoved( size_t );
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEITEM_H
