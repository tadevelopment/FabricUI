//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "ViewportOptionsEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Commands/CommandRegistry.h>
#include <FabricUI/Commands/CommandException.h>
#include <FabricUI/Commands/KLCommandManager.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FabricUI/OptionsEditor/RTValDictModelItem.h>
#include <FabricUI/OptionsEditor/RTValArrayModelItem.h>
#include <FabricUI/OptionsEditor/OptionsEditorHelpers.h>

using namespace FabricUI;
using namespace Util;
using namespace Commands;
using namespace Viewports;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

SetViewportModelItemCommand::SetViewportModelItemCommand()
  : BaseSetRTValModelItemCommand()
{
}

SetViewportModelItemCommand::~SetViewportModelItemCommand()
{
}

void SetViewportModelItemCommand::setSingleOption(
  const QString &editorID,
  const QString &optionsPath,
  RTVal value) 
{
  KLCommandManager *manager = dynamic_cast<KLCommandManager*>(
    CommandManager::GetCommandManager());
   
  RTVal options = getOptions(editorID);

  SetKLSingleOption(
    manager->getClient(), 
    optionsPath,
    value,
    options);
}

RTVal SetViewportModelItemCommand::getOptions(
  const QString &editorID) 
{
  ViewportOptionsEditor *editor = dynamic_cast<ViewportOptionsEditor*>(
    GetOptionsEditor(editorID)
    );
 
  return editor->getViewportParams();
}

QString SetViewportModelItemCommand::getHelp()
{
  QString help = "Sets the value of a Viewport OptionsEditor model item.\n";
  help +=  "Arguments:\n";
  help +=  "- editorID (String): Qt objectName of the option editor / ID of the KL option in the OptionsTargetRegistry\n";
  help +=  "- optionsPath (String): Path of the option in the editor model hierarchy\n";
  help +=  "- newValue (Unknown): New option\n";
  help +=  "- previousValue (Unknown, optional, no-loggable): Previous option";
  return help;
}

ViewportModelItem::ViewportModelItem(
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
}

ViewportModelItem::~ViewportModelItem() 
{
}

void ViewportModelItem::setValue(
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
      KLCommandManager *manager = dynamic_cast<KLCommandManager*>(
        CommandManager::GetCommandManager());

      QMap<QString, RTVal> args;

      args["editorID"] = RTVal::ConstructString(
        manager->getClient(), 
        "Rendering Options");

      args["optionsPath"] = RTVal::ConstructString(
        manager->getClient(), 
        m_path.c_str());

      // might be invalid when changing a Float with the keyboard (as text), for example
      QVariant previousValue = valueAtInteractionBegin.isValid() 
        ? valueAtInteractionBegin 
        : getValue();

      RTVal previousRTValValue = m_options.clone();
      RTVariant::toRTVal( previousValue, previousRTValValue);
      args["previousValue"] = previousRTValValue;
      args["newValue"] = m_options.clone();

      manager->createCommand(
        "setViewportModelItem",
        args);
    }

    catch(Exception &e)
    {
      printf(
        "ViewportModelItem::getRTValOptions: exception: %s\n", 
        e.getDesc_cstr());
    }

    catch (std::string &e) 
    {
      printf(
        "ViewportModelItem::getRTValOptions: exception: %s\n", 
        e.c_str());
    }
  }
}

ViewportOptionsEditor::ViewportOptionsEditor( 
  Client client, 
  QSettings *settings)
  : BaseRTValOptionsEditor(client, "Rendering Options", 0, settings)
{
  CommandFactory<SetViewportModelItemCommand>::Register(
    "setViewportModelItem");
}

ViewportOptionsEditor::~ViewportOptionsEditor()
{
}

RTVal ViewportOptionsEditor::getViewportParams() {

  RTVal res;

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

    res = drawContext.maybeGetMember(
      "viewportParams");
  }

  catch(Exception e)
  {
    printf(
      "ViewportOptionsEditor::getViewportParams : %s\n", 
      e.getDesc_cstr());
  }

  return res;
}

BaseModelItem* ViewportOptionsEditor::constructModel(
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
      return new ViewportModelItem(
        name,
        path,
        editor,
        (void*)&rtValOptions,
        settings);
  }

  catch(Exception &e)
  {
    printf(
      "ViewportOptionsEditor::constructModel: exception: %s\n", 
      e.getDesc_cstr());
  }

  return 0;
}

void ViewportOptionsEditor::resetModel(
  void *options) 
{
  RTVal rtValOptions = getViewportParams();
  BaseRTValOptionsEditor::resetModel(
    (void*)&rtValOptions);
}

void ViewportOptionsEditor::updateModel(
  void *options) 
{
  RTValItem *rtValModel = dynamic_cast<RTValItem*>(
    m_model);

  RTVal rtValOptions = getViewportParams();
  rtValModel->setRTValOptions(rtValOptions);
}
