//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_INTSLIDER_H
#define FABRICUI_VALUEEDITOR_INTSLIDER_H

#include <QSlider>
#include <qevent.h>

namespace FabricUI {
namespace ValueEditor {

class IntSlider : public QSlider {
  Q_OBJECT

public:

  IntSlider( QWidget *parent = 0 );

  // Override the slider positioning to make the slider
  // go directly to the clicked positioning (instead of
  // going there by incremenets)
  virtual void mousePressEvent( QMouseEvent * ) /*override*/;

  virtual void wheelEvent( QWheelEvent *event ) /*override*/;
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_INTSLIDER_H
