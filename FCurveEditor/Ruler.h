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

  qreal m_start, m_end;
  Qt::Orientation m_orientation;

public:

  // TODO : properties
  // - scale : base2, base10, "mixed-grading", etc...
  // - heuristic for chosing the big/small graduations displayed (average spacing?)
  // - labels for large/small numbers : [ "0.5", "0.50123" ] or [ "1000000", "2000000" ] 

  Ruler( Qt::Orientation );
  void setRange( qreal start, qreal end );

protected:
  void paintEvent( QPaintEvent * ) FTL_OVERRIDE;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RULER_H
