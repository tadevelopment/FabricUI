//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include "KLOptionsTargetModelItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Commands/RTValCommandManager.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace FabricCore;
using namespace ValueEditor;
using namespace Application;
using namespace OptionsEditor;

KLOptionsTargetModelItem::KLOptionsTargetModelItem(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor* editor,
  RTVal options) 
  : RTValModelItem(
    name, 
    path, 
    editor, 
    options)
{   
}

KLOptionsTargetModelItem::~KLOptionsTargetModelItem()
{
}

void KLOptionsTargetModelItem::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin) 
{
  try
  {
    if(commit)
    {
      QMap<QString, RTVal> args;
 
      RTVal pathVal = RTVal::ConstructString(
        m_options.getContext(), 
        m_path.c_str());
    
      args["target"] = RTVal::Construct(
        m_options.getContext(), 
        "PathValue",
        1,
        &pathVal);

      // might be invalid when changing a Float with the keyboard (as text), for example
      QVariant previousValue = valueAtInteractionBegin.isValid() 
        ? valueAtInteractionBegin 
        : getValue();

      RTVal prevOptionsCopy = m_options.clone();
      RTVariant::toRTVal(previousValue, prevOptionsCopy);
      args["previousValue.value"] = prevOptionsCopy;

      RTVal optionsCopy = m_options.clone();
      RTVariant::toRTVal(value, optionsCopy);
      args["newValue.value"] = optionsCopy.clone();

      RTValCommandManager *manager = static_cast<RTValCommandManager*>(
        CommandManager::GetCommandManager());

      manager->createCommand(
        "setKLOptionsTargetModelItem",
        args);
    }

    else
    {
      RTValModelItem::setValue(
        value, 
        commit, 
        valueAtInteractionBegin);

      SetKLOptionsTargetSingleOption(
        QString(m_path.c_str()),
        m_options);
    }
  }

  catch(FabricException &e)
  {
    FabricException::Throw(
      "KLOptionsTargetModelItem::getRTValOptions",
      "",
      e.what());
  }

  catch(Exception &e)
  {
    FabricException::Throw(
      "KLOptionsTargetModelItem::getRTValOptions",
      "",
      e.getDesc_cstr());
  }
}
