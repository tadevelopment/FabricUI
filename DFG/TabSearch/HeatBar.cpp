//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "HeatBar.h"

#include <QPainter>

namespace FabricUI {
namespace DFG {
namespace TabSearch {

HeatBar::HeatBar( QWidget *parent )
  : Parent( parent )
  , m_coldColor( Qt::green )
  , m_warmColor( Qt::yellow )
  , m_hotColor( Qt::red )
  , m_warmPos( 0.5f )
  , m_percentage( 0.5f )
{
  setFrameStyle( QFrame::StyledPanel | QFrame::Plain );
  setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

QSize HeatBar::sizeHint() const
{
  return QSize( 32, 10 );
}

void HeatBar::paintEvent( QPaintEvent *event )
{
  QRect r = rect();
  QLinearGradient g( r.left(), 0, r.right(), 0 );
  g.setColorAt( 0.0f, m_coldColor );
  g.setColorAt( m_warmPos, m_warmColor );
  g.setColorAt( 1.0f, m_hotColor );
  r.setRight( r.left() + int( roundf( r.width() * m_percentage ) ) );
  QPainter p( this );
  p.fillRect( r, g );

  Parent::paintEvent( event );
}
  
} // namespace TabSearch
} // namespace DFG
} // namespace FabricUI
