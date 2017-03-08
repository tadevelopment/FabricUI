//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_INTSLIDERVIEWITEM_H
#define FABRICUI_VALUEEDITOR_INTSLIDERVIEWITEM_H

#include "BaseViewItem.h"

class QString;
class QVariant;
class QWidget;

namespace FabricUI {
namespace ValueEditor {

class IntSlider;
class ItemMetadata;
class VELineEdit;

class IntSliderViewItem : public BaseViewItem
{
  Q_OBJECT

public:

  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  IntSliderViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
  virtual void deleteMe() /*override*/
    { delete this; }
  ~IntSliderViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void metadataChanged() /*override*/;
  
private:

  QWidget *m_widget;
  VELineEdit *m_lineEdit;
  IntSlider *m_slider;
  int m_softMinimum;
  int m_softMaximum;
  int m_hardMinimum;
  int m_hardMaximum;
  bool m_isSettingValue;

private slots:

  void onLineEditTextModified( QString text );

  void onSliderPressed();
  void onIntegerValueChanged( int value );
  void onSliderReleased();
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_INTSLIDERVIEWITEM_H
