//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "SHBaseVEEditorOwner.h"
#include "SGObjectPropertyModelItem.h"
#include "SGObjectModelItem.h"
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>
#include <FabricUI/DFG/DFGController.h>

using namespace FabricUI;
using namespace SceneHub;
using namespace ModelItems;


SHBaseVEEditorOwner::SHBaseVEEditorOwner(DFG::DFGWidget * dfgWidget)
  : DFG::DFGVEEditorOwner(dfgWidget)
  , m_objectPropertyItem(0) {
}

SHBaseVEEditorOwner::~SHBaseVEEditorOwner() {
  delete m_objectPropertyItem;
}

SGObjectPropertyModelItem *SHBaseVEEditorOwner::getSGObjectPropertyModelItem() {
  return m_objectPropertyItem;
}

SGObjectModelItem* SHBaseVEEditorOwner::castToSGModelItem(ValueEditor::BaseModelItem *item) {
  return dynamic_cast< SGObjectModelItem * >(item);
}

void SHBaseVEEditorOwner::updateSGObject(const FabricCore::RTVal& sgObject) {
  
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

void SHBaseVEEditorOwner::onSGObjectPropertyItemInserted(FabricUI::ValueEditor::BaseModelItem * item) {
  if(item)
    emit modelItemInserted(m_modelRoot, 0, item->getName().c_str());
}

void SHBaseVEEditorOwner::updateSGObjectProperty(const FabricCore::RTVal& sgObjectProperty) {
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
      printf("SHBaseVEEditorOwner::updateSGObjectProperty: Error: %s\n", e.getDesc_cstr());
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

void SHBaseVEEditorOwner::onSynchronizeCommands() {
  emit synchronizeCommands();
}
