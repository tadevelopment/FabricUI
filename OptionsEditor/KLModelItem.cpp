//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "KLOptionsEditor.h"
#include "KLModelItem.h"
#include "OptionsEditorHelpers.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/Commands/RTValCommandManager.h>

using namespace FabricUI;
using namespace ValueEditor;
using namespace OptionsEditor;
using namespace FabricCore;
using namespace Commands;

KLModelItem::KLModelItem(
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
  KLOptionsEditor* klEditor = dynamic_cast<KLOptionsEditor*>(
    editor);
  
  m_registryID = klEditor->getRegistryID();
  m_client = klEditor->getClient();
}

KLModelItem::~KLModelItem()
{
}

void KLModelItem::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin) 
{
  RTValModelItem::setValue(
    value, 
    commit, 
    valueAtInteractionBegin);

  if(commit)
  {
    try
    {
      QMap<QString, RTVal> args;

      args["registryID"] = RTVal::ConstructString(
        m_client, 
        m_registryID.toUtf8().constData());

      args["optionsPath"] = RTVal::ConstructString(
        m_client, 
        m_path.c_str());

      // might be invalid when changing a Float with the keyboard (as text), for example
      QVariant previousValue = valueAtInteractionBegin.isValid() 
        ? valueAtInteractionBegin 
        : getValue();

      RTVal previousRTValValue = m_options.clone();
      RTVariant::toRTVal( previousValue, previousRTValValue);
      args["previousValue"] = previousRTValValue;
      args["newValue"] = m_options.clone();

      RTValCommandManager *manager = dynamic_cast<RTValCommandManager*>(
        CommandManager::GetCommandManager());

      manager->createRTValCommand(
        "setOptionsModelValue",
        args);
    }

    catch(Exception &e)
    {
      printf(
        "KLModelItem::getRTValOptions: exception: %s\n", 
        e.getDesc_cstr());
    }

    catch (std::string &e) 
    {
      printf(
        "KLModelItem::getRTValOptions: exception: %s\n", 
        e.c_str());
    }
  }
  else
    SetKLOptionsTargetSingleOption(
      m_client,
      m_registryID,
      QString(m_path.c_str()),
      m_options);
}

