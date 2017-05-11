//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QApplication>
#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include <FabricUI/Util/RTValUtil.h>

using namespace FabricCore;
 
namespace FabricUI {
namespace OptionsEditor {

// Qt helpers
inline QMainWindow* GetMainWindow() 
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
  const QString &editorID,
  const QString &title,
  const QString &groupeName)
{
  QMainWindow* mainWindow = GetMainWindow();

  QDockWidget *dock = new QDockWidget(
    title, 
    mainWindow);

  dock->setObjectName(editorID);

  BaseOptionsEditor *optionsEditor = new KLOptionsTargetEditor(
    client,
    editorID,
    editorID);

  dock->setWidget(optionsEditor);

  mainWindow->addDockWidget( 
      Qt::RightDockWidgetArea, 
      dock, 
      Qt::Vertical);

  return dock;
}

QDockWidget * GetOptionsEditorDock(
  const QString &editorID) 
{ 
  QMainWindow* mainWindow = GetMainWindow();
  return mainWindow->findChild<QDockWidget *>(editorID);
}

BaseOptionsEditor* GetOptionsEditor(
  const QString &editorID) 
{
  QDockWidget * dock = GetOptionsEditorDock(editorID);
  BaseOptionsEditor* editor = dock->findChild<BaseOptionsEditor *>();
  return editor;
}

// KL generic editor (use for Viewport options editor too)
inline RTVal GetKLSingleOption_(
  Client client,
  int index,
  QList<QString> &singleOptionPaths,
  RTVal &options) 
{ 
  try
  {
    if(options.isWrappedRTVal()) 
      options = options.getUnwrappedRTVal(); 

    QString optionName = singleOptionPaths[index];

    RTVal key = RTVal::ConstructString(
      client,
      optionName.toUtf8().constData()); 

    if(options.isDict()) 
    {
      RTVal childrenOptions = options.getDictElement( 
        key); 

      if(childrenOptions.isDict() || childrenOptions.isArray())
        return GetKLSingleOption_(
          client,
          index+1, 
          singleOptionPaths, 
          childrenOptions);

      else
        return childrenOptions;
    }

    else if(options.isArray()) 
    {
      // RTVal key = RTVal::ConstructString(
      //   client,
      //   optionName.toUtf8().constData()); 

      // RTVal childrenOptions = rtValOptions->getDictElement( key); 

      // if( Util::RTValUtil::getRTValType(options) == "RTVal[String]" || 
      //     Util::RTValUtil::getRTValType(options) == "RTVal[]")
        
      //   GetKLSingleOption_(
      //     index+1, 
      //     singleOptionPaths, 
      //     singleOption,
      //     childrenOptions);
    }

    else
      return options;
  }

  catch(Exception &e)
  {
    printf(
      "OptionsEditorHelpers::GetKLSingleOption_: exception: %s\n", 
      e.getDesc_cstr());
  }

  RTVal dumb;
  return dumb;
}

inline void SetKLSingleOption_(
  Client client,
  int index,
  QList<QString> &singleOptionPaths,
  RTVal singleOption,
  RTVal options) 
{ 
  if(options.isWrappedRTVal()) 
    options = options.getUnwrappedRTVal(); 

  try
  {
    QString optionName = singleOptionPaths[index];

    RTVal key = RTVal::ConstructString(
      client,
      optionName.toUtf8().constData()); 

    if(options.isDict()) 
    {
      RTVal childrenOptions = options.getDictElement( 
        key); 

      if(childrenOptions.isDict() || childrenOptions.isArray())
        SetKLSingleOption_(
          client,
          index+1, 
          singleOptionPaths, 
          singleOption,
          childrenOptions);

      else
        options.setDictElement(
          key, 
          Util::RTValUtil::forceToKLRTVal(
            singleOption)
          );
    }

    else if(options.isArray()) 
    {
      // RTVal key = RTVal::ConstructString(
      //   client,
      //   optionName.toUtf8().constData()); 

      // RTVal childrenOptions = rtValOptions->getDictElement( key); 

      // if( Util::RTValUtil::getRTValType(options) == "RTVal[String]" || 
      //     Util::RTValUtil::getRTValType(options) == "RTVal[]")
        
      //   SetKLSingleOption_(
      //     index+1, 
      //     singleOptionPaths, 
      //     singleOption,
      //     childrenOptions);
    }
  }

  catch(Exception &e)
  {
    printf(
      "OptionsEditorHelpers::SetKLSingleOption_: exception: %s\n", 
      e.getDesc_cstr());
  }
}

RTVal GetKLSingleOption(
  Client client,
  QString singleOptionPath,
  RTVal options) 
{ 
  QList<QString> singleOptionPaths = 
    singleOptionPath.split('/');
 
  if(options.isWrappedRTVal()) 
    options = options.getUnwrappedRTVal(); 

  return GetKLSingleOption_(
    client,
    1,
    singleOptionPaths,
    options);
}

void SetKLSingleOption(
  Client client,
  QString singleOptionPath,
  RTVal singleOption,
  RTVal options) 
{ 
  QList<QString> singleOptionPaths = 
    singleOptionPath.split('/');
 
  SetKLSingleOption_(
    client,
    1,
    singleOptionPaths,
    singleOption,
    options);
}

// KL OptionsTarget helpers
inline RTVal GetKLOptionsTargetRegistry(
  Client client) 
{
  RTVal optionsTargetRegistry;

  try
  {
    RTVal appOptionsTargetRegistry = RTVal::Construct(
      client,
      "AppOptionsTargetRegistry",
      0, 
      0);

    optionsTargetRegistry = appOptionsTargetRegistry.callMethod(
      "OptionsTargetRegistry",
      "getOptionsTargetRegistry",
      0,
      0);
  }

  catch(Exception &e)
  {
    printf(
      "OptionsEditorHelpers::GetKLOptionsTargetRegistry: exception: %s\n", 
      e.getDesc_cstr());
  }

  return optionsTargetRegistry;
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

    RTVal optionsTargetRegistry = GetKLOptionsTargetRegistry(
      client);
    
    options = optionsTargetRegistry.callMethod(
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

inline void SetKLOptionsTargetOptions(
  Client client,
  QString registryID,
  RTVal options) 
{  
  try
  {
    RTVal registryIDVal = RTVal::ConstructString(
      client,
      registryID.toUtf8().constData());

    RTVal optionsTargetRegistry = GetKLOptionsTargetRegistry(
      client);
    
    RTVal args[2] = 
    {
      RTVal::ConstructString(
        client, 
        registryID.toUtf8().constData())
      ,
      RTVal::Construct(
        client, 
        "RTVal",
        1,
        &options)
    };

    optionsTargetRegistry.callMethod(
      "RTVal", 
      "setTargetOptions",
      2,
      args);
  }

  catch(Exception &e)
  {
    printf(
      "OptionsEditorHelpers::SetKLOptionsTargetOptions: exception: %s\n", 
      e.getDesc_cstr());
  }
}

void SetKLOptionsTargetSingleOption(
  Client client,
  QString registryID,
  QString singleOptionPath,
  RTVal singleOption) 
{ 
  RTVal options = GetKLOptionsTargetOptions(
    client,
    registryID
    );

  SetKLSingleOption(
    client,
    singleOptionPath,
    singleOption,
    options);

  SetKLOptionsTargetOptions(
    client,
    registryID,
    options);
}

} // namespace OptionsEditor 
} // namespace FabricUI
