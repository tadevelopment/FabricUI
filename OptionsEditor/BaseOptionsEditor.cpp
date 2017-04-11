//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "OptionsModel.h"
#include "OptionsDictModel.h"
#include "BaseOptionsEditor.h"
#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>

using namespace FabricUI;
using namespace OptionsEditor;
 
BaseOptionsEditor::BaseOptionsEditor( 
  QUndoStack * undoStack )
  : VETreeWidget()
  , m_undoStack(undoStack)
  , m_model(NULL)
{
  updateOptions();
}

BaseOptionsEditor::~BaseOptionsEditor()
{
  delete m_model;
}

void BaseOptionsEditor::emitValueChanged()
{
  emit valueChanged();
}

QUndoStack* BaseOptionsEditor::getUndoStack()
{
  return m_undoStack;
}

// Currently destroying and rebuilding the whole tree :
// we might want to do incremental updates, to keep the state of the items
void BaseOptionsEditor::updateOptions() {
  if (m_model != NULL) 
  { 
    delete m_model; 
  }
}
