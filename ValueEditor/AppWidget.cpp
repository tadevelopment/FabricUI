//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseViewItem.h"
#include "BaseModelItem.h"
#include "AppWidget.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/Commands/CommandRegistry.h>

using namespace FabricUI;
using namespace ValueEditor;
using namespace FabricCore;

AppWidget::AppWidget(
  BaseViewItem *viewItem)
  : m_viewItem(viewItem)
{
}

AppWidget::~AppWidget()
{
  try
  {
    if(m_klWidget.isValid() && !m_klWidget.isNullObject())
    {
      RTVal baseWidget = RTVal::Create(
        m_klWidget.getContext(),
        "BaseWidget",
        1,
        &m_klWidget
        );

      // To prevent possible leacks.
      baseWidget.callMethod(
        "", 
        "invalidate", 
        0, 
        0
        );

      m_klWidget = RTVal();
    }
  }

  catch(Exception &e)
  {
    printf(
      "AppWidget::~AppWidget: exception: %s\n", 
      e.getDesc_cstr()
      );
  }
}

RTVal AppWidget::getAppWidgetRegistry() 
{
  RTVal widgetRegistry;

  try
  {    
    Client client = Commands::CommandRegistry::GetCommandRegistry()->getFabricClient();
    
    widgetRegistry = RTVal::Create(
      client,
      "AppWidgetRegistry",
      0,
      0
      );

    widgetRegistry = widgetRegistry.callMethod(
      "AppWidgetRegistry",
      "getAppWidgetRegistry",
      0,
      0
      );

    widgetRegistry.callMethod(
      "",
      "registerAppWidgets",
      0,
      0
      );

    widgetRegistry.callMethod(
      "",
      "registerAppTargets",
      0,
      0
      );
  }

  catch(Exception &e)
  {
    printf(
      "AppWidget::getAppWidgetRegistry: exception: %s\n", 
      e.getDesc_cstr()
      );
  }

  return widgetRegistry;
}

QCheckBox* AppWidget::createKLWidget(
  FabricCore::RTVal drivenDataType)
{
  BaseModelItem *modelItem = m_viewItem->getModelItem();

  if(modelItem == NULL)
    return 0;

  try
  {    
    Client client = Commands::CommandRegistry::GetCommandRegistry()->getFabricClient();

    QString cmdName = modelItem->getCommandName();
    RTVal cmdArgs = modelItem->getCommandArgs();

    if(cmdArgs.isValid())
    {
      RTVal widgetArgs[3] =
      {
        drivenDataType,
        
        RTVal::ConstructString( 
          client, 
          cmdName.toUtf8().constData() ),
        
        cmdArgs
      };

      m_klWidget = getAppWidgetRegistry().callMethod(
        "AppWidget",
        "createWidget",
        3,
        widgetArgs
        );

      QString widgetName = m_klWidget.callMethod(
        "String",
        "type",
        0,
        0
        ).getStringCString();

      QCheckBox* checkbox = new QCheckBox(
        widgetName
        );

      QObject::connect(
        checkbox,
        SIGNAL(toggled(bool)),
        this,
        SLOT(setVisible(bool))
        );

      return checkbox;
    }
  }

  catch(Exception &e)
  {
    printf(
      "AppWidget::createKLWidget: exception: %s\n", 
      e.getDesc_cstr()
      );
  }

  return 0;
}

void AppWidget::setVisible(
  bool visibility)
{
  try
  {
    RTVal visibilityVal = RTVal::ConstructBoolean(
      m_klWidget.getContext(), 
      visibility
      );
    
    RTVal baseWidget = RTVal::Create(
      m_klWidget.getContext(),
      "BaseWidget",
      1,
      &m_klWidget
      );

    baseWidget.callMethod(
      "",
      "setVisible",
      1,
      &visibilityVal
      );

    m_viewItem->emitRefreshViewport();
  }

  catch(Exception &e)
  {
    printf(
      "AppWidget::setVisible: exception: %s\n", 
      e.getDesc_cstr());
  }
}
 
void AppWidget::valueChanged(
  RTVal val) 
{
  try
  {
    if(m_klWidget.isValid() && !m_klWidget.isNullObject())
    {
      RTVal rtVal = Util::RTValUtil::rtValToKLRTVal(
        val
        );

      m_klWidget.callMethod(
        "",
        "valueChanged",
        1,
        &rtVal
        );

      m_viewItem->emitRefreshViewport();
    }
  }

  catch(Exception &e)
  {
    printf(
      "AppWidget::valueChanged: exception: %s\n", 
      e.getDesc_cstr()
      );
  }
}
