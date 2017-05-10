//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "RTValModelItem.h"
#include "BaseRTValOptionsEditor.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>
 
using namespace FabricUI;
using namespace FabricCore;
using namespace ValueEditor;
using namespace OptionsEditor;

RTValModelItem::RTValModelItem(
  const std::string &name,
  const std::string &path,
  BaseOptionsEditor* editor,
  void *options,
  QSettings *settings) 
  : BaseSimpleModelItem(name, path, editor)
  , m_settings(settings)
{   
  BaseRTValOptionsEditor* rtValEditor = dynamic_cast<BaseRTValOptionsEditor*>(
    editor);

  m_client = rtValEditor->getClient();

  m_options = *(RTVal *)options;
  if(m_options.isWrappedRTVal()) 
    m_options = m_options.getUnwrappedRTVal(); 

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
  RTVal option = RTVal::Construct(
    m_client,
    "RTVal",
    1,
    &m_options);

  return option;
}

void RTValModelItem::setRTValOptions(
  RTVal options) 
{
  if(options.isWrappedRTVal()) 
    options = options.getUnwrappedRTVal(); 

  setValue(
    toVariant(options));
}
