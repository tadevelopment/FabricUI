//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QMainWindow>
#include <QApplication>
#include "OptionsEditorHelpers.h"
#include "KLOptionsTargetEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Util/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

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
  const QString &editorID,
  const QString &title,
  const QString &groupeName)
{
  QMainWindow* mainWindow = GetMainWindow();

  QDockWidget *dock = new QDockWidget(
    title, 
    mainWindow);

  dock->setObjectName(editorID);

  BaseRTValOptionsEditor *optionsEditor = new KLOptionsTargetEditor(
    editorID);

  dock->setWidget(optionsEditor);

  mainWindow->addDockWidget( 
      Qt::RightDockWidgetArea, 
      dock, 
      Qt::Vertical);

  return dock;
}

QDockWidget* GetOptionsEditorDock(
  const QString &editorID) 
{ 
  QMainWindow* mainWindow = GetMainWindow();
  return mainWindow->findChild<QDockWidget *>(editorID);
}

BaseRTValOptionsEditor* GetOptionsEditor(
  const QString &editorID) 
{
  QDockWidget *dock = GetOptionsEditorDock(editorID);
  BaseRTValOptionsEditor* editor = dock->findChild<BaseRTValOptionsEditor *>();
  return editor;
}

// KL generic editor (use for Viewport options editor too)
inline RTVal GetKLSingleOption(
  int index,
  QList<QString> &singleOptionPaths,
  RTVal &options) 
{ 
  try
  {
    options = Util::RTValUtil::forceToRTVal(options);
 
    QString optionName = singleOptionPaths[index];

    RTVal key = RTVal::ConstructString(
      options.getContext(),
      optionName.toUtf8().constData()); 

    if(options.isDict()) 
    {
      RTVal childrenOptions = options.getDictElement( 
        key); 

      if(childrenOptions.isDict() || childrenOptions.isArray())
        return GetKLSingleOption(
          index+1, 
          singleOptionPaths, 
          childrenOptions);

      else
        return childrenOptions;
    }

    else if(options.isArray()) 
    {
      // RTVal key = RTVal::ConstructString(
      //   context,
      //   optionName.toUtf8().constData()); 

      // RTVal childrenOptions = rtValOptions->getDictElement( key); 

      // if( Util::RTValUtil::getRTValType(options) == "RTVal[String]" || 
      //     Util::RTValUtil::getRTValType(options) == "RTVal[]")
        
      //   GetKLSingleOption(
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
    Util::FabricException::Throw(
      "OptionsEditorHelpers::GetKLSingleOption",
      "",
      e.getDesc_cstr());
  }

  RTVal dumb;
  return dumb;
}

inline void SetKLSingleOption(
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
      singleOption.getContext(),
      optionName.toUtf8().constData()); 

    if(options.isDict()) 
    {
      RTVal childrenOptions = options.getDictElement( 
        key); 

      if(childrenOptions.isDict() || childrenOptions.isArray())
        SetKLSingleOption(
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
      //   singleOption.getContext(),
      //   optionName.toUtf8().constData()); 

      // RTVal childrenOptions = rtValOptions->getDictElement( key); 

      // if( Util::RTValUtil::getRTValType(options) == "RTVal[String]" || 
      //     Util::RTValUtil::getRTValType(options) == "RTVal[]")
        
      //   SetKLSingleOption(
      //     index+1, 
      //     singleOptionPaths, 
      //     singleOption,
      //     childrenOptions);
    }
  }

  catch(Exception &e)
  {
    Util::FabricException::Throw(
      "OptionsEditorHelpers::SetKLSingleOption",
      "",
      e.getDesc_cstr());
  }
}
  
// KL OptionsTarget helpers
RTVal GetKLOptionsTargetRegistry() 
{
  RTVal optionsTargetRegistry;

  try
  {
    RTVal appOptionsTargetRegistry = RTVal::Construct(
      Application::FabricApplicationStates::GetAppStates()->getContext(),
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
    Util::FabricException::Throw(
      "OptionsEditorHelpers::GetKLOptionsTargetRegistry",
      "",
      e.getDesc_cstr());
  }

  return optionsTargetRegistry;
}
 
RTVal GetKLOptionsTargetOptions(
  QString registryID) 
{
  RTVal options;
  
  try
  {
    RTVal registryIDVal = RTVal::ConstructString(
      Application::FabricApplicationStates::GetAppStates()->getContext(),
      registryID.toUtf8().constData());

    RTVal optionsTargetRegistry = GetKLOptionsTargetRegistry();
    
    options = optionsTargetRegistry.callMethod(
      "RTVal", 
      "getTargetOptions",
      1,
      &registryIDVal);
  }

  catch(Exception &e)
  {
    Util::FabricException::Throw(
      "OptionsEditorHelpers::GetKLOptionsTargetOptions",
      "",
      e.getDesc_cstr());
  }

  return options;
}

RTVal GetKLOptionsTargetSingleOption(
  QString path) 
{ 
  RTVal res;

  try
  {
    int index = path.indexOf("/");

    QString registryID = path.midRef(
      0, index).toUtf8().constData();

    RTVal options = GetKLOptionsTargetOptions(
      registryID);

    res = GetKLSingleOption(
      1,
      path.split('/'),
      options);
  }

  catch(Exception &e)
  {
    Util::FabricException::Throw(
      "OptionsEditorHelpers::GetKLOptionsTargetSingleOption",
      "",
      e.getDesc_cstr());
  }

  return res;
}

void SetKLOptionsTargetSingleOption(
  QString path,
  RTVal singleOption) 
{ 
  try
  {
    int index = path.indexOf("/");

    QString registryID = path.midRef(
      0, index).toUtf8().constData();

    RTVal options = GetKLOptionsTargetOptions(
      registryID);

    SetKLSingleOption(
      1,
      path.split('/'),
      singleOption,
      options);
 
    RTVal optionsTargetRegistry = GetKLOptionsTargetRegistry();    

    RTVal args[2] = 
    {
      RTVal::ConstructString(
        options.getContext(), 
        registryID.toUtf8().constData())
      ,
      RTVal::Construct(
        options.getContext(), 
        "RTVal",
        1,
        &options)
    };

    optionsTargetRegistry.callMethod(
      "", 
      "setTargetOptions",
      2,
      args);
  }

  catch(Exception &e)
  {
    Util::FabricException::Throw(
      "OptionsEditorHelpers::SetKLOptionsTargetSingleOption",
      "",
      e.getDesc_cstr());
  }
}

} // namespace OptionsEditor 
} // namespace FabricUI
