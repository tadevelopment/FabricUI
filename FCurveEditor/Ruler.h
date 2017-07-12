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

  // maximum space (in pixels) between 2 major graduations
  Q_PROPERTY( size_t majorGradsPixelSpacing READ majorGradsPixelsSpacing WRITE setMajorGradsPixelSpacing )

  // number of minor graduations between two major graduations
  Q_PROPERTY( qreal majToMinGradsRatio READ majToMinGradsRatio WRITE setMajToMinGradsRatio )

  // the major graduations displayed will be logarithmic multiples of that scale
  Q_PROPERTY( qreal logScale READ logScale WRITE setLogScale )

  Q_PROPERTY( QColor penColor READ penColor WRITE setPenColor )
  Q_PROPERTY( qreal majorPenWidth READ majorPenWidth WRITE setMajorPenWidth )
  Q_PROPERTY( qreal minorPenWidth READ minorPenWidth WRITE setMinorPenWidth )

private:

  qreal m_start, m_end;
  Qt::Orientation m_orientation;
  size_t m_majorGradsPixelSpacing;
  qreal m_majToMinGradsRatio;
  qreal m_logScale;
  QColor m_penColor;
  qreal m_majorPenWidth;
  qreal m_minorPenWidth;

public:

  // TODO : properties
  // - scale : base2, base10, "mixed-grading", etc...
  // - labels for large/small numbers : [ "0.5", "0.50123" ] or [ "1000000", "2000000" ] 

  Ruler( Qt::Orientation );
  void setRange( qreal start, qreal end );

  inline size_t majorGradsPixelsSpacing() const { return m_majorGradsPixelSpacing; }
  inline void setMajorGradsPixelSpacing( size_t s ) { m_majorGradsPixelSpacing = s; this->update(); }
  inline qreal majToMinGradsRatio() const { return m_majToMinGradsRatio; }
  inline void setMajToMinGradsRatio( qreal r ) { m_majToMinGradsRatio = r; this->update(); }
  inline qreal logScale() const { return m_logScale; }
  inline void setLogScale( qreal s ) { if( s > 0 ) { m_logScale = s; this->update(); } }

  inline QColor penColor() const { return m_penColor; }
  inline void setPenColor( QColor c ) { m_penColor = c; this->update(); }
  inline qreal majorPenWidth() const { return m_majorPenWidth; }
  inline void setMajorPenWidth( qreal w ) { m_majorPenWidth = w; this->update(); }
  inline qreal minorPenWidth() const { return m_minorPenWidth; }
  inline void setMinorPenWidth( qreal w ) { m_minorPenWidth = w; this->update(); }

protected:
  void paintEvent( QPaintEvent * ) FTL_OVERRIDE;
};

} // namespace FCurveEditor
} // namespace FabricUI

#endif // FABRICUI_FCURVEEDITOR_RULER_H
