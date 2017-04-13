//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "BaseOptionsEditor.h"
#include "OptionsDictModel.h"

using namespace FabricUI;
using namespace OptionsEditor;
 
BaseOptionsEditor::BaseOptionsEditor( 
  QUndoStack * undoStack)
  : VETreeWidget()
  , m_undoStack(undoStack)
  , m_model(NULL)
{
  updateOptions();
}

BaseOptionsEditor::~BaseOptionsEditor()
{
  if (m_model != NULL) 
  { 
    delete m_model; 
    m_model = NULL;
  }
}

void BaseOptionsEditor::onValueChanged()
{
  emit valueChanged();
}

void BaseOptionsEditor::onValueCommitted(QUndoCommand *cmd)
{
  m_undoStack->push(cmd);
}

// Currently destroying and rebuilding the whole tree :
// we might want to do incremental updates, to keep the state of the items
void BaseOptionsEditor::updateOptions() {
  if (m_model != NULL) 
  { 
    delete m_model; 
    m_model = NULL;
  }
}
