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
  class FCurveEditor;
}

namespace ValueEditor {

class ItemMetaData;

class RTValFCurveViewItem : public BaseViewItem
{
  Q_OBJECT

  class RTValAnimXFCurveDFGController;
  RTValAnimXFCurveDFGController* m_model;
  class Editor;
  Editor* m_editor;
  class ExpandedEditor;
  ExpandedEditor* m_expandedEditor;

public:
  static BaseViewItem *CreateItem(
    QString const& name,
    QVariant const& value,
    ItemMetadata* metaData
  );
  static const int Priority;

  RTValFCurveViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
  );
  ~RTValFCurveViewItem();

  virtual QWidget* getWidget() FTL_OVERRIDE;
  void onModelValueChanged( QVariant const & ) FTL_OVERRIDE;
  void deleteMe() FTL_OVERRIDE { delete this; }

private slots:
  void onViewValueChanged();
  void onEditorInteractionBegin();
  void onEditorInteractionEnd();
  void expand();
  inline void emitInteractionEnd() { emit this->interactionEnd( true ); }
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_RTVALFCURVEVIEWITEM_H
