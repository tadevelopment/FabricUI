//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValDictModelItem.h"
#include "RTValArrayModelItem.h"
#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include "KLOptionsTargetModelItem.h"
#include <FabricUI/Util/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

KLOptionsTargetEditor::KLOptionsTargetEditor(
  const QString &title)
  : BaseRTValOptionsEditor(title, 0)
{
  resetModel();
}

KLOptionsTargetEditor::~KLOptionsTargetEditor() 
{
}

BaseRTValModelItem* KLOptionsTargetEditor::constructModel(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor *editor,
  RTVal options) 
{
  try
  {
    options = RTValUtil::forceToRTVal(options);
 
    if(options.isDict()) 
      return new RTValDictModelItem(
        name,
        path,
        editor,
        options);

    else if(options.isArray())
      return new RTValArrayModelItem(
        name,
        path,
        editor,
        options);

    else
      return new KLOptionsTargetModelItem(
        name,
        path,
        editor,
        options);
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLOptionsTargetEditor::constructModel",
      "",
      e.getDesc_cstr());
  }

  return 0;
}

void KLOptionsTargetEditor::updateModel(
  RTVal options) 
{
  m_model->setRTValOptions(
    GetKLOptionsTargetOptions(
      m_title)
    );
}

void KLOptionsTargetEditor::resetModel(
  RTVal options) 
{
  BaseRTValOptionsEditor::resetModel(
    GetKLOptionsTargetOptions(
      m_title)
    );
}
