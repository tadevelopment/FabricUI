//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "OptionsEditorHelpers.h"
#include "RTValModelItem.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
 
using namespace FabricUI;
using namespace ValueEditor;
using namespace OptionsEditor;
using namespace FabricCore;

RTValModelItem::RTValModelItem(
  const std::string &name,
  const std::string &path,
  BaseOptionsEditor* editor,
  void *options,
  QSettings *settings) 
  : BaseSimpleModelItem(name, path, editor)
  , m_settings(settings)
{   
  m_options = *(RTVal *)options;
  m_originalOptions = m_options.clone();
 
  // Fetching the value from the QSettings
  if (m_settings != 0 && m_settings->contains(m_path.data())) 
  {
    QString settingsValue = m_settings->value( 
      m_path.data() 
      ).value<QString>();

    m_options.setJSON(settingsValue.toUtf8().data());
  }
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

  BaseSimpleModelItem::setValue(
    value, 
    commit, 
    valueAtInteractionBegin);
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
  return m_options;
}

void RTValModelItem::setRTValOptions(
  RTVal options) 
{
  setValue(
    toVariant(options));
}
