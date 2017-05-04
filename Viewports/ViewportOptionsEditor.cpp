//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "ViewportOptionsEditor.h"
 
using namespace FabricUI;
using namespace Viewports;
using namespace OptionsEditor;
using namespace FabricCore;

ViewportOptionsEditor::ViewportOptionsEditor( 
  Client client, 
  QSettings *settings)
  : BaseRTValOptionsEditor(client, "Rendering Options", 0, settings)
{
}

ViewportOptionsEditor::~ViewportOptionsEditor()
{
}

void ViewportOptionsEditor::resetModel(
  void *options) 
{
  RTVal viewportParams;

  try
  {
    // Create a DrawContext object
    m_client.loadExtension(
      "InlineDrawing", 
      "", 
      false);

    RTVal drawContext = RTVal::Create(
      m_client, 
      "DrawContext", 
      0, 0);

    drawContext = drawContext.callMethod( 
      "DrawContext" , 
      "getInstance", 
      0, 0);

    viewportParams = drawContext.maybeGetMember(
      "viewportParams");
  
    BaseRTValOptionsEditor::resetModel(
      (void*)&viewportParams);
  }

  catch(Exception e)
  {
    printf(
      "ViewportOptionsEditor::getRTValOptions : %s\n", 
      e.getDesc_cstr());
  }
}
