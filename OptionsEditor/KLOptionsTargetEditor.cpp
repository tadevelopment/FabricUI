//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValDictModelItem.h"
#include "RTValArrayModelItem.h"
#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include "KLOptionsTargetModelItem.h"

using namespace FabricUI;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

KLOptionsTargetEditor::KLOptionsTargetEditor(
  Client client,
  const QString &editorID,
  const QString &title,
  QSettings *settings)
  : BaseRTValOptionsEditor(client, title, 0, settings)
  , m_editorID(editorID)
{
  resetModel();
}

KLOptionsTargetEditor::~KLOptionsTargetEditor() 
{
}

QString KLOptionsTargetEditor::geteditorID() 
{ 
  return m_editorID; 
}

BaseModelItem* KLOptionsTargetEditor::constructModel(
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
      return new KLOptionsTargetModelItem(
        name,
        path,
        editor,
        (void*)&rtValOptions,
        settings);
  }

  catch(Exception &e)
  {
    printf(
      "KLOptionsTargetEditor::constructModel: exception: %s\n", 
      e.getDesc_cstr());
  }

  return 0;
}

void KLOptionsTargetEditor::updateModel(
  void *options) 
{
  RTValItem *rtValModel = dynamic_cast<RTValItem*>(
    m_model);

  RTVal rtValOptions = GetKLOptionsTargetOptions(
    m_client, 
    m_editorID);

  rtValModel->setRTValOptions(
    rtValOptions);
}

void KLOptionsTargetEditor::resetModel(
  void *options) 
{
  RTVal rtValOptions = GetKLOptionsTargetOptions(
    m_client, 
    m_editorID);

  BaseOptionsEditor::resetModel(
    (void*)&rtValOptions);
}
