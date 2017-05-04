//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QApplication>
#include "KLOptionsEditor.h"
#include "OptionsEditorHelpers.h"
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricCore;
 
namespace FabricUI {
namespace OptionsEditor {

QMainWindow* GetMainWindow() 
{
  foreach(QWidget *wiget, QApplication::topLevelWidgets())
  { 
    QMainWindow *mainWindow = dynamic_cast<QMainWindow *>(wiget);
    if(mainWindow != 0)
      return mainWindow;
  }
  return 0;
}

QDockWidget* CreateOptionsEditor( 
  Client client,
  const QString &registryID,
  const QString &title,
  const QString &groupeName)
{
  QMainWindow* mainWindow = GetMainWindow();

  QDockWidget *dock = new QDockWidget(
    title, 
    mainWindow);

  dock->setObjectName(registryID);

  BaseOptionsEditor *optionsEditor = new KLOptionsEditor(
    client,
    registryID,
    registryID);

  dock->setWidget(optionsEditor);

  mainWindow->addDockWidget( 
      Qt::RightDockWidgetArea, 
      dock, 
      Qt::Vertical);

  return dock;
}
 
QDockWidget* GetOptionsEditorDockByTitle(
  const QString &title
  ){
  return 0;
}

QList<QDockWidget*> GetOptionsEditorDocks() 
{ 
  QMainWindow* mainWindow = GetMainWindow();
  return mainWindow->findChildren<QDockWidget *>();
}

QDockWidget * GetOptionsEditorDock(
  const QString &registryID) 
{ 
  QMainWindow* mainWindow = GetMainWindow();
  return mainWindow->findChild<QDockWidget *>(registryID);
}

BaseOptionsEditor* GetOptionsEditor(
  const QString &registryID) 
{
  QDockWidget * dock = GetOptionsEditorDock(registryID);
  BaseOptionsEditor* editor = dock->findChild<BaseOptionsEditor *>();
  return editor;
}

RTVal GetKLOptionsEditorTargetRegistry(
  Client client) 
{
  RTVal optionsEditorTargetRegistry;

  try
  {
    RTVal appOptionsEditorTargetRegistry = RTVal::Construct(
      client,
      "AppOptionsEditorTargetRegistry",
      0, 
      0);

    optionsEditorTargetRegistry = appOptionsEditorTargetRegistry.callMethod(
      "OptionsEditorTargetRegistry",
      "getOptionsEditorTargetRegistry",
      0,
      0);
  }

  catch(Exception &e)
  {
    printf(
      "OptionsEditorHelpers::GetKLOptionsEditorTargetRegistry: exception: %s\n", 
      e.getDesc_cstr());
  }

  return optionsEditorTargetRegistry;
}
 
RTVal GetKLOptionsTargetOptions(
  Client client,
  QString registryID) 
{
  RTVal options;
  
  try
  {
    RTVal registryIDVal = RTVal::ConstructString(
      client,
      registryID.toUtf8().constData());

    RTVal optionsEditorTargetRegistry = GetKLOptionsEditorTargetRegistry(
      client);
    
    options = optionsEditorTargetRegistry.callMethod(
      "RTVal", 
      "getTargetOptions",
      1,
      &registryIDVal);
  }

  catch(Exception &e)
  {
    printf(
      "OptionsEditorHelpers::GetKLOptionsTargetOptions: exception: %s\n", 
      e.getDesc_cstr());
  }

  return options;
}

void SetKLOptionsTargetSingleOption(
  Client client,
  QString registryID,
  QString optionPath,
  RTVal rtVal) 
{ 
  try
  {
    RTVal optionsEditorTargetRegistry = GetKLOptionsEditorTargetRegistry(
      client);

    RTVal args[3] = 
    {
      RTVal::ConstructString(
        client, 
        registryID.toUtf8().constData())
      ,
      RTVal::ConstructString(
        client, 
        optionPath.toUtf8().constData())
      ,
      RTVal::Construct(
        client, 
        "RTVal",
        1,
        &rtVal)
    };

    optionsEditorTargetRegistry.callMethod(
      "",
      "setTargetSingleOptions",
      3, 
      args);
  }

  catch(Exception &e)
  {
    printf(
      "OptionsEditorHelpers::SetKLOptionsTargetSingleOption: exception: %s\n", 
      e.getDesc_cstr());
  }
}

} // namespace OptionsEditor 
} // namespace FabricUI
