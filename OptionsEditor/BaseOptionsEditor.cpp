//
// Copyright(c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseOptionsEditor.h"
  
using namespace FabricUI;
using namespace ValueEditor;
using namespace OptionsEditor;

BaseOptionsEditor::BaseOptionsEditor(
  const QString &title,
  void *options,
  QSettings *settings)
  : VETreeWidget()
  , m_title(title)
  , m_settings(settings)
  , m_model(0)
{
}

BaseOptionsEditor::~BaseOptionsEditor()
{
  if(m_model != 0) 
  { 
    delete m_model; 
    m_model = 0;
  }
}

void BaseOptionsEditor::modelUpdated()
{
  emit updated();
}

void BaseOptionsEditor::resetModel(
  void *options) 
{
  if(m_model != 0) 
  { 
    delete m_model; 
    m_model = 0;
  }
   
  m_model = constructModel(
    m_title.toUtf8().constData(),
    "",
    this,
    options, 
    m_settings);
 
  onSetModelItem(
    m_model);
}

void BaseOptionsEditor::updateModel(
  void *options) 
{
  throw(
    "BaseOptionsEditor::updateModel must be overridden");
}

BaseModelItem* BaseOptionsEditor::constructModel(
  const std::string &name,
  const std::string &path,
  BaseOptionsEditor *editor,
  void *options,
  QSettings *settings) 
{
  throw(
    "BaseOptionsEditor::constructModel must be overridden");
}
