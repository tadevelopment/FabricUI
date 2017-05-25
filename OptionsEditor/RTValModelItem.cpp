//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValModelItem.h"
#include <FabricUI/Util/RTValUtil.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>

using namespace FabricUI;
using namespace Util;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

RTValModelItem::RTValModelItem(
  const std::string &name,
  const std::string &path,
  BaseRTValOptionsEditor* editor,
  RTVal options,
  QSettings *settings) 
  : BaseRTValModelItem(name, path)
  , m_settings(settings)
{   
  m_options = RTValUtil::forceToRTVal(options);
  m_originalOptions = m_options.clone();
 
  // Fetching the value from the QSettings
  if(m_settings != 0 && m_settings->contains(m_path.data())) 
  {
    QString settingsValue = m_settings->value( 
      m_path.data() 
      ).value<QString>();

    m_options.setJSON(settingsValue.toUtf8().data());
  }

  QObject::connect(
    this,
    SIGNAL(updated()),
    editor,
    SLOT(modelUpdated())
    );
}

RTValModelItem::~RTValModelItem()
{
}

QVariant RTValModelItem::getValue() 
{
  return toVariant(m_options.clone());
}

void RTValModelItem::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin) 
{
  // RTVariant::toRTVal might change the pointer in ioVal,
  // so we copy it to make sure that m_options will always point to the same place
  RTVal optionsCopy = m_options.clone();

  RTVariant::toRTVal( 
    value, 
    optionsCopy);

  m_options.assign(optionsCopy);

  // Storing the value in the Settings
  if(m_settings != 0)
    m_settings->setValue( 
      m_path.data(),
      QString(m_options.getJSON().getStringCString())
      );

  emitModelValueChanged( getValue() );
  emit updated();
}

void RTValModelItem::resetToDefault() 
{
  setValue( 
    toVariant(m_originalOptions), 
    true, 
    getValue());
}

RTVal RTValModelItem::getRTValOptions()
{
  return RTValUtil::forceToKLRTVal(m_options);
}

void RTValModelItem::setRTValOptions(
  RTVal options) 
{
  setValue(
    toVariant(RTValUtil::forceToRTVal(options))
    );
}
