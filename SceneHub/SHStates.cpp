/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHStates.h"
 
using namespace FabricCore;
using namespace FabricUI::SceneHub;

SHStates::SHStates(Client client):
  m_client(client),
  m_activeSHGLScene(0) {
  try 
  {
    m_shStateVal = RTVal::Create(client, "SHStates", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHStates::SHStates: exception: %s\n", e.getDesc_cstr());
  }
}

FabricCore::Client SHStates::getClient() {
  return m_client; 
}

FabricCore::RTVal SHStates::getSelectedObjects() {
  RTVal selectedObject;
  try 
  {
    selectedObject = m_shStateVal.callMethod("SGObject[]", "getSelectedObjects", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHStates::getSelectedObjects: exception: %s\n", e.getDesc_cstr());
  }
  return selectedObject;
}

bool SHStates::isInspectingSGObject() {
  bool result = false;
  try 
  {
    result = m_shStateVal.callMethod("Boolean", "isInspectingObject", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHStates::isInspectingSGObject: exception: %s\n", e.getDesc_cstr());
  }
  return result;
}

bool SHStates::isInspectingSGObjectProperty() {
  bool result = false;
  try 
  {
    result = m_shStateVal.callMethod("Boolean", "isInspectingObjectProperty", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHStates::isInspectingSGObjectProperty: exception: %s\n", e.getDesc_cstr());
  }
  return result;
}

bool SHStates::isInspectingSGCanvasOperator() {
  bool result = false;
  try 
  {
    result = m_shStateVal.callMethod("Boolean", "isInspectingSGCanvasOperator", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHStates::isInspectingSGCanvasOperator: exception: %s\n", e.getDesc_cstr());
  }
  return result;
}

FabricCore::RTVal SHStates::getOptionsRef() {

  FabricCore::RTVal result;
  try 
  {
    result = m_shStateVal.maybeGetMemberRef("options");
  }
  catch(Exception e)
  {
    printf("SHStates::getOptionsRef: exception: %s\n", e.getDesc_cstr());
  }
  return result;
}

void SHStates::updateFromOptions() {

  try 
  {
    m_shStateVal.callMethod( "", "updateSceneHubFromOptions", 0, 0 );
    emit sceneChanged();
  }
  catch(Exception e)
  {
    printf("SHStates::updateFromOptions: exception: %s\n", e.getDesc_cstr());
  }
}

FabricCore::RTVal SHStates::getInspectedSGObject() {
  FabricCore::RTVal result;
  try 
  {
    FabricCore::RTVal validRTVal = FabricCore::RTVal::ConstructBoolean(m_client, false);
    result = m_shStateVal.callMethod("SGObject", "getInspectedObject", 1, &validRTVal);
    if(!validRTVal.getBoolean())
      result = FabricCore::RTVal();
  }
  catch(Exception e)
  {
    printf("SHStates::getInspectedSGObject: exception: %s\n", e.getDesc_cstr());
  }
  return result;
}

FabricCore::RTVal SHStates::getInspectedSGObjectProperty() {
  FabricCore::RTVal result;
  try 
  {
    FabricCore::RTVal validRTVal = FabricCore::RTVal::ConstructBoolean(m_client, false);
    result = m_shStateVal.callMethod("SGObjectProperty", "getInspectedObjectProperty", 1, &validRTVal);
    if(!validRTVal.getBoolean())
      result = FabricCore::RTVal();
  }
  catch(Exception e)
  {
    printf("SHStates::getInspectedSGObjectProperty: exception: %s\n", e.getDesc_cstr());
  }
  return result;
}

FabricCore::RTVal SHStates::getInspectedSGCanvasOperator() {
  FabricCore::RTVal result;
  try 
  {
    result = m_shStateVal.callMethod("SGCanvasOperator", "getInspectedObjectPropertyGenerator", 0, 0);
    if(result.isNullObject())
      result = FabricCore::RTVal();
  }
  catch(Exception e)
  {
    printf("SHStates::getInspectedSGCanvasOperator: exception: %s\n", e.getDesc_cstr());
  }
  return result;
}

SHGLScene* SHStates::getActiveScene() {
  return m_activeSHGLScene;
}

void SHStates::onStateChanged() {
  try 
  {
    if(m_shStateVal.callMethod("Boolean", "sceneHierarchyChanged", 0, 0).getBoolean())
      emit sceneHierarchyChanged();
    if(m_shStateVal.callMethod("Boolean", "sceneChanged", 0, 0).getBoolean())
      emit sceneChanged();
    if(m_shStateVal.callMethod("Boolean", "selectionChanged", 0, 0).getBoolean())
      emit selectionChanged();
  }
  catch(Exception e)
  {
    printf("SHStates::onStateChanged: exception: %s\n", e.getDesc_cstr());
  }
}

void SHStates::onInspectSelectedSGObject() {
  try 
  {
    FabricCore::RTVal sgObjectlist = getSelectedObjects();
    if(sgObjectlist.getArraySize() > 0)
      onInspectedSGObject(sgObjectlist.getArrayElement(0)); 
  }
  catch(Exception e)
  {
    printf("SHStates::onInspectedCurrentSGObject: exception: %s\n", e.getDesc_cstr());
  }
}

void SHStates::onInspectedSGObject(FabricCore::RTVal sgObject) {
  try 
  {
    if(m_shStateVal.callMethod("Boolean", "setInspectedObject", 1, &sgObject).getBoolean())
      emit inspectedChanged();
  }
  catch(Exception e)
  {
    printf("SHStates::onInspectedSGObject: exception: %s\n", e.getDesc_cstr());
  }
}

void SHStates::onInspectedSGObjectProperty(FabricCore::RTVal sgObjectProperty) {
  try 
  {
    if(m_shStateVal.callMethod("Boolean", "setInspectedObjectProperty", 1, &sgObjectProperty).getBoolean())
      emit inspectedChanged();
  }
  catch(Exception e)
  {
    printf("SHStates::onInspectedSGObjectProperty: exception: %s\n", e.getDesc_cstr());
  }
}

void SHStates::onInspectedSGObjectPropertyGenerator(FabricCore::RTVal sgObjectProperty) {
  try 
  {
    if(m_shStateVal.callMethod("Boolean", "setInspectedObjectPropertyGenerator", 1, &sgObjectProperty).getBoolean())
      emit inspectedChanged();
  }
  catch(Exception e)
  {
    printf("SHStates::onInspectedSGObjectPropertyGenerator: exception: %s\n", e.getDesc_cstr());
  }
}

void SHStates::onActiveSceneChanged(SHGLScene* scene) {
  try 
  {
    m_activeSHGLScene = scene;
    FabricCore::RTVal sceneRTVal = scene->getSHGLScene();
    if(m_shStateVal.callMethod("Boolean", "setActiveScene", 1, &sceneRTVal).getBoolean())
      emit activeSceneChanged();
  }
  catch(Exception e)
  {
    printf("SHStates::onActiveSceneChanged: exception: %s\n", e.getDesc_cstr());
  }
}

void SHStates::onFrameChanged(int frame) {
  try 
  {
    RTVal arg = RTVal::ConstructSInt32(getClient(), frame);
    if(m_shStateVal.callMethod("Boolean", "setFrame", 1, &arg).getBoolean())
      emit sceneChanged();
  }
  catch(Exception e)
  {
    printf("SHStates::onFrameChanged: exception: %s\n", e.getDesc_cstr());
  }
}

void SHStates::clearSelection() {
  try 
  {
    m_shStateVal.callMethod("", "clearSelection", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHStates::clearSelection: exception: %s\n", e.getDesc_cstr());
  }
  onStateChanged();
}

void SHStates::addSGObjectToSelection(FabricCore::RTVal sgObject) {
  try 
  {
    m_shStateVal.callMethod("", "addObjectToSelection", 1, &sgObject);
  }
  catch(Exception e)
  {
    printf("SHStates::addSGObjectToSelection: exception: %s\n", e.getDesc_cstr());
  }
  onStateChanged();
}

void SHStates::removeSGObjectFromSelection(FabricCore::RTVal sgObject) {
  try 
  {
    m_shStateVal.callMethod("", "removeObjectFromSelection", 1, &sgObject);
  }
  catch(Exception e)
  {
    printf("SHStates::removeSGObjectFromSelection: exception: %s\n", e.getDesc_cstr());
  }
  onStateChanged();
}

void SHStates::addSGObjectPropertyToSelection(FabricCore::RTVal sgObject) {
  try 
  {
    m_shStateVal.callMethod("", "addObjectPropertyToSelection", 1, &sgObject);
  }
  catch(Exception e)
  {
    printf("SHStates::addSGObjectPropertyToSelection: exception: %s\n", e.getDesc_cstr());
  }
  onStateChanged();
}

void SHStates::removeSGObjectPropertyFromSelection(FabricCore::RTVal sgObject) {
  try 
  {
    m_shStateVal.callMethod("", "removeObjectPropertyFromSelection", 1, &sgObject);
  }
  catch(Exception e)
  {
    printf("SHStates::removeSGObjectPropertyFromSelection: exception: %s\n", e.getDesc_cstr());
  }
  onStateChanged();
}

void SHStates::addSGObjectPropertyGeneratorToSelection(FabricCore::RTVal sgObject) {
  try 
  {
    m_shStateVal.callMethod("", "addObjectPropertyGeneratorToSelection", 1, &sgObject);
  }
  catch(Exception e)
  {
    printf("SHStates::addSGObjectPropertyGeneratorToSelection: exception: %s\n", e.getDesc_cstr());
  }
  onStateChanged();
}

void SHStates::removeSGObjectPropertyGeneratorFromSelection(FabricCore::RTVal sgObject) {
  try 
  {
    m_shStateVal.callMethod("", "removeObjectPropertyGeneratorFromSelection", 1, &sgObject);
  }
  catch(Exception e)
  {
    printf("SHStates::removeSGObjectPropertyGeneratorFromSelection: exception: %s\n", e.getDesc_cstr());
  }
  onStateChanged();
}
