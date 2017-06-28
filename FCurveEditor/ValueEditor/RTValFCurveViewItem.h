//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_RTVALFCURVEVIEWITEM_H
#define FABRICUI_VALUEEDITOR_RTVALFCURVEVIEWITEM_H

#include <FabricUI/ValueEditor/BaseViewItem.h>
#include <FTL/Config.h>

namespace FabricUI {

namespace FCurveEditor
{
  class RTValAnimXFCurveModel;
  class FCurveEditor;
}

namespace ValueEditor {

class ItemMetaData;

class AnimCurveViewItem : public BaseViewItem
{
  Q_OBJECT

  FabricUI::FCurveEditor::RTValAnimXFCurveModel* m_model;
  FabricUI::FCurveEditor::FCurveEditor* m_editor;

public:
  static BaseViewItem *CreateItem(
    QString const& name,
    QVariant const& value,
    ItemMetadata* metaData
  );
  static const int Priority;

  AnimCurveViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
  );
  ~AnimCurveViewItem();

  virtual QWidget* getWidget() FTL_OVERRIDE;
  void onModelValueChanged( QVariant const & ) FTL_OVERRIDE;
  void deleteMe() FTL_OVERRIDE { delete this; }

private slots:
  void onViewValueChanged();
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_RTVALFCURVEVIEWITEM_H
