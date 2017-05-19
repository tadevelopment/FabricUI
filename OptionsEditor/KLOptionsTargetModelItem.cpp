//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include "KLOptionsTargetModelItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/Commands/RTValCommandManager.h>

using namespace FabricUI;
using namespace Commands;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

KLOptionsTargetModelItem::KLOptionsTargetModelItem(
  const std::string &name,
  const std::string &path,
  BaseOptionsEditor* editor,
  void *options,
  QSettings *settings) 
  : RTValModelItem(
    name, 
    path, 
    editor, 
    options, 
    settings)
{   
  KLOptionsTargetEditor* klEditor = dynamic_cast<KLOptionsTargetEditor*>(
    editor);
  
  m_editorID = klEditor->geteditorID();
  m_client = klEditor->getClient();
}

KLOptionsTargetModelItem::~KLOptionsTargetModelItem()
{
}

void KLOptionsTargetModelItem::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin) 
{
  if(commit)
  {
    try
    {
      QMap<QString, RTVal> args;

      args["editorID"] = RTVal::ConstructString(
        m_client, 
        m_editorID.toUtf8().constData());

      args["optionsPath"] = RTVal::ConstructString(
        m_client, 
        m_path.c_str());

      // might be invalid when changing a Float with the keyboard (as text), for example
      QVariant previousValue = valueAtInteractionBegin.isValid() 
        ? valueAtInteractionBegin 
        : getValue();

      RTVal prevOptionsCopy = m_options.clone();
      RTVariant::toRTVal(previousValue, prevOptionsCopy);
      args["previousValue"] = prevOptionsCopy;

      RTVal optionsCopy = m_options.clone();
      RTVariant::toRTVal(value, optionsCopy);

      args["newValue"] = optionsCopy.clone();

      RTValCommandManager *manager = dynamic_cast<RTValCommandManager*>(
        CommandManager::GetCommandManager());

      manager->createCommand(
        "setKLOptionsTargetModelItem",
        args);
    }

    catch(Exception &e)
    {
      printf(
        "KLOptionsTargetModelItem::getRTValOptions: exception: %s\n", 
        e.getDesc_cstr());
    }

    catch (std::string &e) 
    {
      printf(
        "KLOptionsTargetModelItem::getRTValOptions: exception: %s\n", 
        e.c_str());
    }
  }
  else
  {
    RTValModelItem::setValue(
      value, 
      commit, 
      valueAtInteractionBegin);

    SetKLOptionsTargetSingleOption(
      m_client,
      m_editorID,
      QString(m_path.c_str()),
      m_options);
  }
}

