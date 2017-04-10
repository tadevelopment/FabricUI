//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "OptionsModel.h"
#include <FabricUI/ValueEditor/QVariantRTVal.h>

using namespace FabricUI;
using namespace OptionsEditor;
using namespace ValueEditor;

const char namePath_Separator = '/';

OptionsModel::OptionsModel(
  const QString &name,
  FabricCore::RTVal value,
  QSettings* settings,
  const QString &namePath,
  BaseOptionsEditor* editor) 
  : BaseModelItem()
  , m_name(name)
  , m_namePath(namePath + namePath_Separator + name)
  , m_val(value)
  , m_originalValue(value.clone())
  , m_settings(settings)
  , m_editor(editor)
{
  // Fetching the value from the QSettings
  if (m_settings->contains(m_namePath)) 
  {
    QString settingsValue = m_settings->value( 
      m_namePath 
      ).value<QString>();

    m_val.setJSON( 
      settingsValue.toUtf8().data() 
      );

    m_editor->emitValueChanged();
  }
}

OptionsModel::~OptionsModel()
{
}

QVariant OptionsModel::getValue() 
{
  return toVariant(m_val.clone());
}

void OptionsModel::setValue(
  QVariant value) 
{
  // RTVariant::toRTVal might change the pointer in ioVal,
  // so we copy it to make sure that m_val will always point to the same place
  FabricCore::RTVal m_valCopy = m_val.clone();
  FabricUI::ValueEditor::RTVariant::toRTVal( value, m_valCopy );
  m_val.assign( m_valCopy );

  // Storing the value in the Settings
  m_settings->setValue( m_namePath, QString( m_val.getJSON().getStringCString() ));

  // Updating the UI
  m_editor->emitValueChanged();
  emitModelValueChanged( getValue() );
}

void OptionsModel::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin) 
{
  // might be invalid when changing a Float with the keyboard (as text), for example
  QVariant previousValue = valueAtInteractionBegin.isValid() ? valueAtInteractionBegin : getValue();

  setValue( value );

  if( commit ) {
    m_editor->getUndoStack()->push(
      new OptionUndoCommand(
        QString("Changed the option \"" + m_namePath + "\""),
        previousValue,
        value,
        this
      )
    );
  }
}

bool OptionsModel::hasDefault() 
{ 
  return true; 
}

void OptionsModel::resetToDefault() 
{

  setValue( toVariant( m_originalValue ), true, getValue() );
}

FTL::CStrRef OptionsModel::getName() 
{ 
  return m_name.toUtf8().constData(); 
}