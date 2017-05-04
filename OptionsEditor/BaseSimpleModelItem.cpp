//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseOptionsEditor.h"
#include "BaseSimpleModelItem.h"
 
using namespace FabricUI;
using namespace ValueEditor;
using namespace OptionsEditor;
 
BaseSimpleModelItem::BaseSimpleModelItem(
  const std::string &name,
  const std::string &path,
  BaseOptionsEditor* editor,
  void *options,
  QSettings *settings)
  : BaseOptionsModelItem(name, path)
{
  QObject::connect(
    this,
    SIGNAL(updated()),
    editor,
    SLOT(modelUpdated())
    );
}

BaseSimpleModelItem::~BaseSimpleModelItem()
{
}

QVariant BaseSimpleModelItem::getValue() 
{
  throw(
    "BaseSimpleModelItem::getValue must be overridden");
}

void BaseSimpleModelItem::setValue(
  QVariant value,
  bool commit,
  QVariant valueAtInteractionBegin) 
{
  emitModelValueChanged( getValue() );
  emit updated();
}

void BaseSimpleModelItem::resetToDefault() 
{
  throw(
    "BaseSimpleModelItem::resetToDefault must be overridden");
}
