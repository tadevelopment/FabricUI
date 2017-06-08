//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include <QMainWindow>
#include "OptionsEditorHelpers.h"
#include <FabricUI/Util/QtUtil.h>
#include "KLOptionsTargetEditor.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Viewports/ViewportWidget.h>
#include <FabricUI/Application/FabricException.h>
#include <FabricUI/Application/FabricApplicationStates.h>

using namespace FabricCore;
 
namespace FabricUI {
namespace OptionsEditor {

QDockWidget* CreateOptionsEditor( 
  const QString &editorID,
  const QString &title,
  const QString &groupeName)
{
  QMainWindow* mainWindow = Util::QtUtil::getMainWindow();
  if(mainWindow == 0)
    Application::FabricException::Throw(
      "OptionsEditorHelpers::CreateOptionsEditor",
      "mainWindow is null");

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

  Viewports::ViewportWidget *viewport = Util::QtUtil::getQWidget<Viewports::ViewportWidget>();
  if(viewport == 0)
    Application::FabricException::Throw(
      "OptionsEditorHelpers::CreateOptionsEditor",
      "Viewport is null");

  QObject::connect(
    viewport,
    SIGNAL(initComplete()),
    optionsEditor,
    SLOT(resetModel())
    );

  QObject::connect(
    optionsEditor,
    SIGNAL(updated()),
    viewport,
    SLOT(redraw())
    );  
 
  return dock;
}

// KL generic editor (use for Viewport options editor too)
inline RTVal GetKLSingleOption(
  int index,
  QList<QString> &singleOptionPaths,
  RTVal &options) 
{ 
  FABRIC_CATCH_BEGIN();

  options = Util::RTValUtil::toRTVal(options);

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

    // if( Util::RTValUtil::getType(options) == "RTVal[String]" || 
    //     Util::RTValUtil::getType(options) == "RTVal[]")
      
    //   GetKLSingleOption(
    //     index+1, 
    //     singleOptionPaths, 
    //     singleOption,
    //     childrenOptions);
  }

  else
    return options;

  FABRIC_CATCH_END("OptionsEditorHelpers::GetKLSingleOption");

  return RTVal();
}

inline void SetKLSingleOption(
  int index,
  QList<QString> &singleOptionPaths,
  RTVal singleOption,
  RTVal options) 
{ 
  FABRIC_CATCH_BEGIN();

  if(options.isWrappedRTVal()) 
    options = options.getUnwrappedRTVal(); 
 
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
        Util::RTValUtil::toKLRTVal(
          singleOption)
        );
  }

  else if(options.isArray()) 
  {
    // RTVal key = RTVal::ConstructString(
    //   singleOption.getContext(),
    //   optionName.toUtf8().constData()); 

    // RTVal childrenOptions = rtValOptions->getDictElement( key); 

    // if( Util::RTValUtil::getType(options) == "RTVal[String]" || 
    //     Util::RTValUtil::getType(options) == "RTVal[]")
      
    //   SetKLSingleOption(
    //     index+1, 
    //     singleOptionPaths, 
    //     singleOption,
    //     childrenOptions);
  }

  FABRIC_CATCH_END("OptionsEditorHelpers::SetKLSingleOption");
}
  
// KL OptionsTarget helpers
RTVal GetKLOptionsTargetRegistry() 
{
  FABRIC_CATCH_BEGIN();

  RTVal appOptionsTargetRegistry = RTVal::Construct(
    Application::FabricApplicationStates::GetAppStates()->getContext(),
    "AppOptionsTargetRegistry",
    0, 0);

  return appOptionsTargetRegistry.callMethod(
    "OptionsTargetRegistry",
    "getOptionsTargetRegistry",
    0, 0);

  FABRIC_CATCH_END("OptionsEditorHelpers::GetKLOptionsTargetRegistry");

  return RTVal();
}
 
RTVal GetKLOptionsTargetOptions(
  QString registryID) 
{
  FABRIC_CATCH_BEGIN();

  RTVal registryIDVal = RTVal::ConstructString(
    Application::FabricApplicationStates::GetAppStates()->getContext(),
    registryID.toUtf8().constData());
  
  return GetKLOptionsTargetRegistry().callMethod(
    "RTVal", 
    "getTargetOptions",
    1,
    &registryIDVal);

  FABRIC_CATCH_END("OptionsEditorHelpers::GetKLOptionsTargetRegistry");

  return RTVal();
}

RTVal GetKLOptionsTargetSingleOption(
  QString path) 
{ 
  FABRIC_CATCH_BEGIN();

  int index = path.indexOf("/");

  QString registryID = path.midRef(
    0, index).toUtf8().constData();

  RTVal options = GetKLOptionsTargetOptions(
    registryID);

  return GetKLSingleOption(
    1,
    path.split('/'),
    options);

  FABRIC_CATCH_END("OptionsEditorHelpers::GetKLOptionsTargetSingleOption");

  return RTVal();
}

void SetKLOptionsTargetSingleOption(
  QString path,
  RTVal singleOption) 
{ 
  FABRIC_CATCH_BEGIN();

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

  GetKLOptionsTargetRegistry().callMethod(
    "", 
    "setTargetOptions",
    2,
    args);

  FABRIC_CATCH_END("OptionsEditorHelpers::SetKLOptionsTargetSingleOption");
}

} // namespace OptionsEditor 
} // namespace FabricUI
