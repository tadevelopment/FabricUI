//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "IntSlider.h"

#include <math.h>
#include <algorithm>
#include <QStyleOptionSlider>
#include <QMouseEvent>

#include <FTL/AutoSet.h>

using namespace FabricUI::ValueEditor;

IntSlider::IntSlider( QWidget *parent )
  : QSlider( parent )
  , m_min( 0 )
  , m_max( 100 )
  , m_value(0)
  , m_isSettingValue( false )
{
  setResolution( m_min, m_max );
  setFocusPolicy(Qt::StrongFocus);

  // Default to horizontal orientation
  setOrientation( Qt::Horizontal );

  connect( this, SIGNAL( valueChanged( int ) ),
           this, SLOT( onValueChanged( int ) ) );
}

void IntSlider::mousePressEvent( QMouseEvent *event )
{
  // Taken pretty much verbatim from:
  // http://stackoverflow.com/questions/11132597/qslider-mouse-direct-jump
  QStyleOptionSlider opt;
  initStyleOption( &opt );
  QRect sr = style()->subControlRect( QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this );

  if (event->button() == Qt::LeftButton &&
        sr.contains( event->pos() ) == false)
  {
    int max = maximum();

    int newVal = 0;
    if ( orientation() == Qt::Vertical )
    {
      int h = height();
      if ( h > 1 )
      {
        --h;
        newVal += (max * (h - event->y()) + h/2) / h;
      }
    }
    else
    {
      int w = width();
      if ( w > 1 )
      {
        --w;
        newVal += (max * event->x() + w/2) / w;
      }
    }

    if ( invertedAppearance() )
      setValue( max - newVal );
    else
      setValue( newVal );

    event->accept();
  }
  QSlider::mousePressEvent( event );
}

void IntSlider::wheelEvent( QWheelEvent *event )
{
  // [FE-5997] inspired by http://stackoverflow.com/questions/5821802/qspinbox-inside-a-qscrollarea-how-to-prevent-spin-box-from-stealing-focus-when
  if (!hasFocus())
  {
    event->ignore();
    return;
  }

  wheelEvent( event );
}

void IntSlider::setResolution( int min, int max )
{
  int resolution = max - min;
  setRange( 0, resolution );
  m_max = max;
  m_min = min;
  setIntegerValue( m_value, false );
}

void IntSlider::setIntegerValue( int newIntegerValue, bool emitSignal )
{
  FTL::AutoSet<bool> settingValue(m_isSettingValue, true);

  m_value = newIntegerValue;
  newIntegerValue = std::max( m_min, std::min( m_max, newIntegerValue ) );
  double ratio = (double)(newIntegerValue - m_min) / (double)(m_max - m_min);
  int newIntValue = int( round( ratio * maximum() ) );
  if ( value() != newIntValue ) {
    setValue( newIntValue );
  }
  if(emitSignal)
    emit integerValueChanged( m_value );
}

int IntSlider::integerValue()
{
  return m_value;
}

int IntSlider::toInteger( int value )
{
  double ratio = double( value ) / double( maximum() );
  return int( round( (1.0 - ratio) * m_min + ratio * m_max ) );
}

void IntSlider::onValueChanged( int value ) {
  if(m_isSettingValue)
    return;
  m_value = toInteger( value );
  emit integerValueChanged( m_value );
}
