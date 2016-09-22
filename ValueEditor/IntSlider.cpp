//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "IntSlider.h"
#include <QStyleOptionSlider>

using namespace FabricUI::ValueEditor;

IntSlider::IntSlider( QWidget *parent )
{
  setFocusPolicy(Qt::StrongFocus);

  // Default to horizontal orientation
  setOrientation( Qt::Horizontal );
}

void IntSlider::mousePressEvent( QMouseEvent *event )
{
  // [FE-] taken from FabricUI::ValueEditor::DoubleSlider::mousePressEvent().
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
