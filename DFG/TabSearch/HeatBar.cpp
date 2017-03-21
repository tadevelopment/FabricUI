//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "HeatBar.h"

#include <QDebug>
#include <QPainter>

namespace FabricUI {
namespace DFG {
namespace TabSearch {

HeatBar::HeatBar( QWidget *parent )
  : Parent( parent )
  , m_coldColor( Qt::darkYellow )
  , m_warmColor( Qt::yellow )
  , m_hotColor( Qt::green )
  , m_warmPos( 0.33f )
  , m_percentage( 0.5f )
  , m_nbBars( 5 )
  , m_smooth( true )
  , m_spacingWidth( 1 )
{
  this->setObjectName( "HeatBar" );

  this->setAutoFillBackground( true );
  setFrameStyle( QFrame::Plain );
  setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

QColor Interpolate( const QColor& a, const QColor&b, qreal v )
{
  return QColor(
    int( ( 1 - v ) * a.red() + v * b.red() ),
    int( ( 1 - v ) * a.green() + v * b.green() ),
    int( ( 1 - v ) * a.blue() + v * b.blue() ),
    int( ( 1 - v ) * a.alpha() + v * b.alpha() )
  );
}

QColor HeatBar::interpolateColor( const qreal percentage ) const
{
  return percentage <= warmPos() ?
    Interpolate( m_coldColor, m_warmColor, ( warmPos() == 0 ? 1 : percentage / warmPos() ) ) :
    Interpolate( m_warmColor, m_hotColor, ( warmPos() == 1 ? 0 : ( percentage - warmPos() ) / ( 1 - warmPos() ) ) )
  ;
}

void HeatBar::paintEvent( QPaintEvent *event )
{
  const QRect r = contentsRect();
  QPainter p( this );
  if( this->nbBars() <= 1 )
  {
    QLinearGradient g( r.left(), 0, r.right(), 0 );
    g.setColorAt( 0.0f, m_coldColor );
    g.setColorAt( m_warmPos, m_warmColor );
    g.setColorAt( 1.0f, m_hotColor );
    QRect barR = r;
    barR.setRight( r.left() + int( roundf( r.width() * m_percentage ) ) );
    if( smooth() )
      p.fillRect( barR, g );
    else
      p.fillRect( barR, interpolateColor( m_percentage ) );
  }
  else
  {
    int totalBarsWidth = r.width() - (this->nbBars() - 1) * m_spacingWidth;

    // qDebug() << "START m_spacingWidth=" << m_spacingWidth;
    for( int barI = 0; barI < ( m_percentage * this->nbBars() ); barI++ )
    {
      int leftX = r.left() + int( round( qreal(barI) * qreal(totalBarsWidth) / qreal(this->nbBars()) ) ) + barI * m_spacingWidth;
      int rightX = r.left() + int( round( qreal(barI + 1) * qreal(totalBarsWidth) / qreal(this->nbBars()) ) ) + barI * m_spacingWidth;
      // qDebug() << "leftX=" << leftX << " rightX=" << rightX;

      QRect barR = r;
      barR.setLeft( leftX ); barR.setRight( rightX - 1 );
      if( !smooth() )
        p.fillRect( barR, interpolateColor( ( leftX + rightX ) / ( 2 * r.width() ) ) );
      else
      {
        QColor leftColor = interpolateColor( leftX / r.width() );
        QColor rightColor = interpolateColor( rightX / r.width() );
        qreal midX = warmPos() * r.width();
        QLinearGradient g( leftX, 0, rightX, 0 );
        g.setColorAt( 0.0f, leftColor );
        g.setColorAt( 1.0f, rightColor );
        if( midX > leftX && midX < rightX )
          g.setColorAt( ( midX - leftX ) / ( rightX - leftX ), warmColor() );
        p.fillRect( barR, g );
      }
    }
    // qDebug() << "END";
  }

  Parent::paintEvent( event );
}
  
} // namespace TabSearch
} // namespace DFG
} // namespace FabricUI
