//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "ViewportOptionsEditor.h"
#include <FabricUI/OptionsEditor/OptionsDictModel.h>

using namespace FabricUI;
using namespace Viewports;
using namespace Bases;

ViewportOptionsEditor::ViewportOptionsEditor( 
  FabricCore::Client& client, 
  QUndoStack& undoStack )
  : Bases::BaseOptionsEditor(client, undoStack)
{
  try
  {
    // Create a DrawContext object
    client.loadExtension("InlineDrawing", "", false);
    m_drawContext = FabricCore::RTVal::Create(client, "DrawContext", 0, 0);
    m_drawContext = m_drawContext.callMethod( "DrawContext" , "getInstance", 0, NULL);
    updateOptions();
  }
  catch(FabricCore::Exception e)
  {
    printf("ViewportOptionsEditor : %s\n", e.getDesc_cstr());
  }
}

// Currently destroying and rebuilding the whole tree :
// we might want to do incremental updates, to keep the state of the items
void ViewportOptionsEditor::updateOptions() {

  Bases::BaseOptionsEditor::updateOptions();

  FabricCore::RTVal dict = m_drawContext.maybeGetMember("viewportParams");

  m_model = new OptionsDictModel(
    "Rendering Options",
    dict,
    &m_settings,
    "",
    *this
  );

  this->onSetModelItem(m_model);
}
