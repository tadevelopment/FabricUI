//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLOptionsEditor.h"
#include "KLModelItem.h"
#include "OptionsEditorHelpers.h"
#include "RTValDictModelItem.h"
#include "RTValArrayModelItem.h"
#include <FabricUI/Commands/KLCommandManager.h>

using namespace FabricUI;
using namespace FabricCore;
using namespace OptionsEditor;
using namespace Commands;
using namespace ValueEditor;

KLOptionsEditor::KLOptionsEditor(
  Client client,
  const QString &registryID,
  const QString &title,
  QSettings *settings)
  : BaseRTValOptionsEditor(client, title, 0, settings)
  , m_registryID(registryID)
{
  resetModel();
}

KLOptionsEditor::~KLOptionsEditor() 
{
}

QString KLOptionsEditor::getRegistryID() 
{ 
  return m_registryID; 
}

BaseModelItem* KLOptionsEditor::constructModel(
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
      return new KLModelItem(
        name,
        path,
        editor,
        (void*)&rtValOptions,
        settings);
  }

  catch(Exception &e)
  {
    printf(
      "KLOptionsEditor::constructModel: exception: %s\n", 
      e.getDesc_cstr());
  }

  return 0;
}

void KLOptionsEditor::updateModel(
  void *options) 
{
  RTValItem *rtValModel = dynamic_cast<RTValItem*>(
    m_model);

  RTVal rtValOptions = GetKLOptionsTargetOptions(
    m_client, 
    m_registryID);

  rtValModel->setRTValOptions(
    rtValOptions);
}
 
void KLOptionsEditor::resetModel(
  void *options) 
{
  RTVal rtValOptions = GetKLOptionsTargetOptions(
    m_client, 
    m_registryID);

  BaseOptionsEditor::resetModel(
    (void*)&rtValOptions);
}