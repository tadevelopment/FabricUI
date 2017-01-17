//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "SHVEEditorOwner.h"
#include "SGObjectPropertyModelItem.h"
#include "SGObjectModelItem.h"
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>
#include <FabricUI/DFG/DFGController.h>

using namespace FabricUI;
using namespace SceneHub;
using namespace ModelItems;


SHVEEditorOwner::SHVEEditorOwner(DFG::DFGWidget * dfgWidget, SHStates* shStates)
  : DFG::DFGVEEditorOwner(dfgWidget)
  , m_shStates(shStates) 
  , m_objectPropertyItem(0) {
}

SHVEEditorOwner::~SHVEEditorOwner() {
  delete m_objectPropertyItem;
}

SGObjectPropertyModelItem *SHVEEditorOwner::getSGObjectPropertyModelItem() {
  return m_objectPropertyItem;
}

void SHVEEditorOwner::updateSGObject(const FabricCore::RTVal& sgObject) {
  
  bool isValid = true;
  bool structureChanged = true;

  SGObjectModelItem * objectItem = castToSGModelItem(m_modelRoot);
  if(objectItem)
    objectItem->updateFromScene(sgObject, isValid, structureChanged);

  if(!structureChanged)
    return;

  // Currently we don't support incremental structure changes; we just rebuild all
  m_valueEditor->clear();
  if(m_modelRoot) 
  {
    delete m_modelRoot;
    m_modelRoot = 0;
  }

  if(m_objectPropertyItem) 
  {
    delete m_objectPropertyItem;
    m_objectPropertyItem = 0;
  }

  objectItem = 0;

  if(sgObject.isValid() && isValid) 
  {
    objectItem = new SGObjectModelItem(getDFGController()->getClient(), sgObject);
    QObject::connect(objectItem, SIGNAL(synchronizeCommands()), this, SLOT(onSynchronizeCommands()));
    QObject::connect(objectItem, SIGNAL(propertyItemInserted(FabricUI::ValueEditor::BaseModelItem *)), this, SLOT(onSGObjectPropertyItemInserted(FabricUI::ValueEditor::BaseModelItem *)));
  }

  m_modelRoot = objectItem;
  emit replaceModelRoot(m_modelRoot);
}

void SHVEEditorOwner::onStructureChanged() {
  DFG::DFGVEEditorOwner::onStructureChanged();

  // refresh!
  SGObjectModelItem * objectItem = castToSGModelItem(m_modelRoot);
  if(objectItem) 
  {
    objectItem->onStructureChanged();
    emit replaceModelRoot(m_modelRoot);
  }
}

void SHVEEditorOwner::onInspectChanged() {
  if(m_shStates->isInspectingSGObject() || m_shStates->isInspectingSGCanvasOperator())
  {
    FabricCore::RTVal sgObject = m_shStates->getInspectedSGObject();
    updateSGObject(sgObject);
  }

  else if(m_shStates->isInspectingSGObjectProperty())
  { 
    FabricCore::RTVal sgObjectProperty = m_shStates->getInspectedSGObjectProperty();
    updateSGObjectProperty(sgObjectProperty);
  }
}

void SHVEEditorOwner::onSceneChanged() {
  SGObjectModelItem *objectItem = castToSGModelItem(m_modelRoot);
  if(objectItem) 
  {
    //Important: take a value copy since passed by ref and sgObject might be deleted
    FabricCore::RTVal sgObject = objectItem->getSGObject();
    updateSGObject(sgObject);
  } 
  else if(m_objectPropertyItem) 
  {
    //Important: take a value copy since passed by ref and sgObject might be deleted
    FabricCore::RTVal sgObjectProperty = m_objectPropertyItem->getSGObjectProperty();
    updateSGObjectProperty(sgObjectProperty);
  }
}

void SHVEEditorOwner::onSynchronizeCommands() {
  emit synchronizeCommands();
}

void SHVEEditorOwner::onSGObjectPropertyItemInserted(FabricUI::ValueEditor::BaseModelItem * item) {
  if(item)
    emit modelItemInserted(m_modelRoot, 0, item->getName().c_str());
}

void SHVEEditorOwner::onSidePanelInspectRequested() {
  FTL::CStrRef execPath = getDFGController()->getExecPath();
  if(execPath.empty())
    emit canvasSidePanelInspectRequested();
}

SGObjectModelItem* SHVEEditorOwner::castToSGModelItem(ValueEditor::BaseModelItem *item) {
  return dynamic_cast< SGObjectModelItem * >(item);
}

void SHVEEditorOwner::updateSGObjectProperty(const FabricCore::RTVal& sgObjectProperty) {
  bool isValid = true;
  //bool structureChanged = true;

  if(m_objectPropertyItem) 
  {
    // Check if it is the same property
    bool sameProperty = false;
    try 
    {
      FabricCore::RTVal prevSGObjectProperty = m_objectPropertyItem->getSGObjectProperty();
      FabricCore::RTVal newSGObjectProperty = sgObjectProperty;
      sameProperty = newSGObjectProperty.callMethod("Boolean", "equals_noContext", 1, &prevSGObjectProperty).getBoolean();
    }
    catch(FabricCore::Exception e) 
    {
      printf("SHVEEditorOwner::updateSGObjectProperty: Error: %s\n", e.getDesc_cstr());
    }
    if(sameProperty) 
    {
      m_objectPropertyItem->updateFromScene();
      return;
    }
  }

  // Currently we don't support incremental changes; just rebuild all
  m_valueEditor->clear();

  if(m_modelRoot) 
  {
    delete m_modelRoot;
    m_modelRoot = 0;
  }

  if(m_objectPropertyItem) 
  {
    delete m_objectPropertyItem;
    m_objectPropertyItem = 0;
  }

  if(sgObjectProperty.isValid() && isValid) 
  {
    m_objectPropertyItem = new SGObjectPropertyModelItem(getDFGController()->getClient(), sgObjectProperty, true);
    QObject::connect(m_objectPropertyItem, SIGNAL(modelValueChanged(QVariant const &)), this, SLOT(onModelValueChanged(QVariant const &)));
    QObject::connect(m_objectPropertyItem, SIGNAL(synchronizeCommands()), this, SLOT(onSynchronizeCommands()));
  }
  emit replaceModelRoot(m_objectPropertyItem);
}
