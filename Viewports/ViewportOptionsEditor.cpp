//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "ViewportOptionsEditor.h"

using namespace FabricUI::Viewports;

ViewportOptionsEditor::OptionsModel::OptionsModel(FabricCore::RTVal drawContext, ViewportOptionsEditor& editor)
  : drawContext(drawContext), editor(editor)
{

}

QVariant ViewportOptionsEditor::OptionsModel::getValue()
{
  try
  {
    // Get the DrawContext singleton
    this->drawContext = this->drawContext.callMethod("DrawContext", "getInstance", 0, 0);
    FabricCore::RTVal dict = this->drawContext.maybeGetMemberRef("viewportParams");
    return toVariant( dict ); // Returns its dictionary of parameters
  }
  catch(FabricCore::Exception e)
  {
    printf("OptionsModel::getValue : %s\n", e.getDesc_cstr());
  }
  return QVariant();
}

void ViewportOptionsEditor::OptionsModel::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin
)
{
  emit this->editor.valueChanged();
}

ViewportOptionsEditor::ViewportOptionsEditor( FabricCore::Client& client )
  : VETreeWidget()
{
   try
  {
    // Create a DrawContext object
    client.loadExtension("InlineDrawing", "", false);
    FabricCore::RTVal drawContext = FabricCore::RTVal::Create(client, "DrawContext", 0, 0);

    this->onSetModelItem(new OptionsModel(drawContext, *this));
  }
  catch(FabricCore::Exception e)
  {
    printf("OptionsModel::ViewportOptionsEditor : %s\n", e.getDesc_cstr());
  }
}

ViewportOptionsEditor::~ViewportOptionsEditor()
{

}
