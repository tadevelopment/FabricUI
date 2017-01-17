//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_SHOPTIONSEDITOR_H
#define FABRICUI_VALUEEDITOR_SHOPTIONSEDITOR_H

#include <FabricCore.h>
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/SceneHub/SHStates.h>

namespace FabricUI {
namespace SceneHub {

  class SHOptionsModel : public FabricUI::ValueEditor::BaseModelItem {

    SHStates& shStates;

  public:
    SHOptionsModel( SHStates& shStates ) : shStates(shStates) {}

    virtual QVariant getValue();

    virtual void setValue(
      QVariant value,
      bool commit,
      QVariant valueAtInteractionBegin
    );
  };

  class SHOptionsEditor : public FabricUI::ValueEditor::VETreeWidget
  {
    Q_OBJECT

  public:
    SHOptionsEditor( SHStates& shStates );
    virtual ~SHOptionsEditor();
  };
}
}

#endif // FABRICUI_VALUEEDITOR_SHOPTIONSEDITOR_H
