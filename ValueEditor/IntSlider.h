//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_INTSLIDER_H
#define FABRICUI_VALUEEDITOR_INTSLIDER_H

#include <QSlider>
#include <qevent.h>

namespace FabricUI {
namespace ValueEditor {

class IntSlider : public QSlider {
  Q_OBJECT

  int m_min;
  int m_max;

public:

  IntSlider( QWidget *parent = 0 );

  // Override the slider positioning to make the slider
  // go directly to the clicked positioning (instead of
  // going there by incremenets)
  virtual void mousePressEvent( QMouseEvent * ) /*override*/;

  virtual void wheelEvent( QWheelEvent *event ) /*override*/;

  void setResolution( int min, int max );

  void setIntegerValue( int value, bool emitSignal = true );
  int integerValue();
  int toInteger( int value );

  int min() const
    { return m_min; }
  int max() const
    { return m_max; }

signals :
  void integerValueChanged( int value);

public slots:
  void onValueChanged( int value );

private:

  int m_value;
  bool m_isSettingValue;
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_INTSLIDER_H
