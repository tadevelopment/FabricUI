//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QFrame>
#include <FTL/Config.h>
#include <algorithm>
#include <math.h>

namespace FabricUI {
namespace DFG {
namespace TabSearch {

class HeatBar : public QFrame
{
  Q_OBJECT
  Q_PROPERTY(QColor coldColor READ coldColor WRITE setColdColor DESIGNABLE true)
  Q_PROPERTY(QColor warmColor READ warmColor WRITE setWarmColor DESIGNABLE true)
  Q_PROPERTY(QColor hotColor READ hotColor WRITE setHotColor DESIGNABLE true)
  Q_PROPERTY(qreal warmPos READ warmPos WRITE setWarmPos DESIGNABLE true)
  Q_PROPERTY(int nbBars READ nbBars WRITE setNbBars DESIGNABLE true)
  Q_PROPERTY(bool smooth READ smooth WRITE setSmooth DESIGNABLE true)
  Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing DESIGNABLE true)

  typedef QFrame Parent;

public:

  HeatBar( QWidget *parent = 0 );

  QColor coldColor() const
    { return m_coldColor; }
  void setColdColor( QColor coldColor )
  {
    m_coldColor = coldColor;
    update();
  }

  QColor warmColor() const
    { return m_warmColor; }
  void setWarmColor( QColor warmColor )
  {
    m_warmColor = warmColor;
    update();
  }

  QColor hotColor() const
    { return m_hotColor; }
  void setHotColor( QColor hotColor )
  {
    m_hotColor = hotColor;
    update();
  }

  qreal warmPos() const
    { return m_warmPos; }
  void setWarmPos( qreal warmPos )
  {
    m_warmPos = warmPos;
    update();
  }

  int nbBars() const { return m_nbBars; }
  void setNbBars( int nbBars ) { m_nbBars = nbBars; update(); }

  bool smooth() const { return m_smooth; }
  void setSmooth( const bool smooth )
  { m_smooth = smooth; update(); }

  qreal spacing() const { return m_spacing; }
  void setSpacing( const qreal spacing ) { m_spacing = spacing; update(); }

  QColor interpolateColor( const qreal percentage ) const;

  void set( qreal value, qreal minValue, qreal maxValue )
  {
    if ( minValue >= maxValue )
      m_percentage = 1.0f;
    else
    {
      value = std::min( maxValue, std::max( minValue, value ) );
      m_percentage = (value - minValue ) / ( maxValue - minValue );
    }
    update();
  }

  QSize minimumSizeHint() const FTL_OVERRIDE;

protected:

  virtual void paintEvent( QPaintEvent *event );

private:

  QColor m_coldColor;
  QColor m_warmColor;
  QColor m_hotColor;
  qreal m_warmPos;
  qreal m_percentage;
  int m_nbBars;
  bool m_smooth;
  qreal m_spacing;
};

} // namespace TabSearch
} // namespace DFG
} // namespace FabricUI
