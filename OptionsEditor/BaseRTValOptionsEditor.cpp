//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValModelItem.h"
#include "RTValDictModelItem.h"
#include "RTValArrayModelItem.h"
#include "BaseRTValOptionsEditor.h"
#include <FabricUI/ValueEditor/BaseModelItem.h>
#include "./Commands/OptionEditorCommandRegistration.h"

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

BaseRTValOptionsEditor::BaseRTValOptionsEditor(
  Client client,
  const QString &title,
  void *options,
  QSettings *settings)
  : BaseOptionsEditor(title, options, settings)
  , m_client(client)
{
  OptionEditorCommandRegistration::RegisterCommands();
}

BaseRTValOptionsEditor::~BaseRTValOptionsEditor() 
{
}

Client BaseRTValOptionsEditor::getClient() 
{
  return m_client;
}
 
BaseModelItem* BaseRTValOptionsEditor::constructModel(
  const std::string &name,
  const std::string &path,
  BaseOptionsEditor *editor,
  void *options,
  QSettings *settings) 
{
  try
  {
    RTVal rtValOptions = *(RTVal*)options;

    if(rtValOptions.isWrappedRTVal()) 
      rtValOptions = rtValOptions.getUnwrappedRTVal(); 
      
    if(rtValOptions.isDict()) 
      return new RTValDictModelItem(
        name,
        path,
        editor,
        (void*)&rtValOptions,
        settings);

    else if(rtValOptions.isArray())
      return new RTValArrayModelItem(
        name,
        path,
        editor,
        (void*)&rtValOptions,
        settings);

    else
      return new RTValModelItem(
        name,
        path,
        editor,
        (void*)&rtValOptions,
        settings);
  }

  catch(Exception &e)
  {
    printf(
      "BaseRTValOptionsEditor::constructModel: exception: %s\n", 
      e.getDesc_cstr());
  }

  return 0;
}
