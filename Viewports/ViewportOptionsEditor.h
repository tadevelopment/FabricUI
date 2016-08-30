//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_VIEWPORTOPTIONSEDITOR_H
#define FABRICUI_VALUEEDITOR_VIEWPORTOPTIONSEDITOR_H

#include <FabricCore.h>
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>

namespace FabricUI {
namespace Viewports {

  class ViewportOptionsEditor : public FabricUI::ValueEditor::VETreeWidget
  {
    Q_OBJECT

    class OptionsModel : public FabricUI::ValueEditor::BaseModelItem {

      /// DrawContext owning the Viewport Options
      FabricCore::RTVal drawContext;

      /// Reference used to update the editor Widget
      ViewportOptionsEditor& editor;

    public:
      OptionsModel(FabricCore::RTVal drawContext, ViewportOptionsEditor& editor);

      virtual QVariant getValue();

      virtual void setValue(
        QVariant value,
        bool commit,
        QVariant valueAtInteractionBegin
      );
    };

  public:
    ViewportOptionsEditor( FabricCore::Client& client );
    virtual ~ViewportOptionsEditor();

  signals:
    /// When the value of one option has changed
    void valueChanged();
  };
}
}

#endif // FABRICUI_VALUEEDITOR_VIEWPORTOPTIONSEDITOR_H
