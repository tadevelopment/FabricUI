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

public:

  FCurveEditor();
  void setCurve( AbstractFCurveModel* );
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEDITOR_H
