//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValDictModelItem.h"
#include "RTValArrayModelItem.h"
#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include "KLOptionsTargetModelItem.h"
#include <FabricUI/Application/FabricException.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

KLOptionsTargetEditor::KLOptionsTargetEditor(
  const QString &title)
  : BaseRTValOptionsEditor(title, 0)
{
  setObjectName(title);
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
  FABRIC_CATCH_BEGIN();

  options = RTValUtil::toRTVal(options);

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

  FABRIC_CATCH_END("KLOptionsTargetEditor::constructModel");

  return 0;
}

void KLOptionsTargetEditor::updateModel(
  RTVal options) 
{  
  FABRIC_CATCH_BEGIN();

  m_model->setRTValOptions( GetKLOptionsTargetOptions(  m_title) );

  FABRIC_CATCH_END("KLOptionsTargetEditor::updateModel");
}

void KLOptionsTargetEditor::resetModel(
  RTVal options) 
{
  FABRIC_CATCH_BEGIN();
  
  BaseRTValOptionsEditor::resetModel( GetKLOptionsTargetOptions( m_title) );
  
  FABRIC_CATCH_END("KLOptionsTargetEditor::resetModel");
}
