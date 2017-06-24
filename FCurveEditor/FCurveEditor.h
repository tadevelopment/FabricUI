//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_FCURVEDITOR_H
#define FABRICUI_FCURVEEDITOR_FCURVEDITOR_H

#include <FabricUI/FCurveEditor/AbstractFCurveModel.h>
#include <QGraphicsWidget>

namespace FabricUI
{
namespace FCurveEditor
{

class FCurveEditor : public QGraphicsWidget
{
  Q_OBJECT

  AbstractFCurveModel* m_curve;
  class FCurveShape;
  FCurveShape* m_curveShape;
  class HandleWidget;
  std::vector<HandleWidget*> m_handles;

  void addHandle( size_t );

public:
  FCurveEditor();
  void setCurve( AbstractFCurveModel* );

private slots:
  void onHandleMoved( size_t );
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEDITOR_H
