//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_FCURVEEDITOR_RULER_H
#define FABRICUI_FCURVEEDITOR_RULER_H

#include <QFrame>
#include <FTL/Config.h>

namespace FabricUI
{
namespace FCurveEditor
{

class Ruler : public QFrame
{
  Q_OBJECT

  typedef QFrame Parent;

private:

  float m_min, m_max;
  Qt::Orientation m_orientation;

public:
  Ruler( Qt::Orientation );
  void setRange( float min, float max );

protected:
  void paintEvent( QPaintEvent * ) FTL_OVERRIDE;
};

} // namespace FCurveEditor
} // namespace ValueEditor 

#endif // FABRICUI_FCURVEEDITOR_RULER_H
