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

  AbstractFCurveModel* m_model;
  QGraphicsScene* m_scene;
  FCurveItem* m_curveItem;

public:
  FCurveEditor();
  ~FCurveEditor();
  void setModel( AbstractFCurveModel* );
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_FCURVEEDITOR_H
