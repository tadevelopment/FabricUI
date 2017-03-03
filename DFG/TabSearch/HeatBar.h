//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QFrame>

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

  void set( qreal value, qreal minValue, qreal maxValue )
  {
    if ( minValue >= maxValue )
      m_percentage = 0.5f;
    else
    {
      value = std::min( maxValue, std::max( minValue, value ) );
      m_percentage = (value - minValue ) / ( maxValue - minValue );
    }
    update();
  }

  virtual QSize sizeHint() const;

protected:

  virtual void paintEvent( QPaintEvent *event );

private:

  QColor m_coldColor;
  QColor m_warmColor;
  QColor m_hotColor;
  qreal m_warmPos;
  qreal m_percentage;
};

} // namespace TabSearch
} // namespace DFG
} // namespace FabricUI
