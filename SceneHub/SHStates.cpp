/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHStates.h"
#include <FabricUI/Util/macros.h>

using namespace FabricCore;
using namespace FabricUI::SceneHub;

SHStates::SHStates(Client client):
  m_client(client),
  m_activeSHGLScene(0) {
  FABRIC_TRY("SHStates::SHStates",
    m_shStateVal = RTVal::Create(client, "SHStates", 0, 0);
 );
}

FabricCore::Client SHStates::getClient() {
  return m_client; 
}

FabricCore::RTVal SHStates::getSelectedObjects() {
  RTVal selectedObject;
  FABRIC_TRY_IGNORE("SHStates::getSelectedObjects",
    selectedObject = m_shStateVal.callMethod("SGObject[]", "getSelectedObjects", 0, 0);
 );
  return selectedObject;
}

bool SHStates::isInspectingSGObject() {
  bool result = false;
  FABRIC_TRY_IGNORE("SHStates::isInspectingSGObject",
    result = m_shStateVal.callMethod("Boolean", "isInspectingObject", 0, 0).getBoolean();
 );
  return result;
}

bool SHStates::isInspectingSGObjectProperty() {
  bool result = false;
  FABRIC_TRY_IGNORE("SHStates::isInspectingSGObjectProperty",
    result = m_shStateVal.callMethod("Boolean", "isInspectingObjectProperty", 0, 0).getBoolean();
 );
  return result;
}

bool SHStates::isInspectingSGCanvasOperator() {
  bool result = false;
  FABRIC_TRY_IGNORE("SHStates::isInspectingSGCanvasOperator",
    result = m_shStateVal.callMethod("Boolean", "isInspectingSGCanvasOperator", 0, 0).getBoolean();
 );
  return result;
}

FabricCore::RTVal SHStates::getInspectedSGObject() {
  FabricCore::RTVal result;
  FABRIC_TRY_IGNORE("SHStates::getInspectedSGObject",
    FabricCore::RTVal validRTVal = FabricCore::RTVal::ConstructBoolean(m_client, false);
    result = m_shStateVal.callMethod("SGObject", "getInspectedObject", 1, &validRTVal);
    if(!validRTVal.getBoolean())
      result = FabricCore::RTVal();
 );
  return result;
}

FabricCore::RTVal SHStates::getInspectedSGObjectProperty() {
  FabricCore::RTVal result;
  FABRIC_TRY_IGNORE("SHStates::getInspectedSGObjectProperty",
    FabricCore::RTVal validRTVal = FabricCore::RTVal::ConstructBoolean(m_client, false);
    result = m_shStateVal.callMethod("SGObjectProperty", "getInspectedObjectProperty", 1, &validRTVal);
    if(!validRTVal.getBoolean())
      result = FabricCore::RTVal();
 );
  return result;
}

FabricCore::RTVal SHStates::getInspectedSGCanvasOperator() {
  FabricCore::RTVal result;
  FABRIC_TRY_IGNORE("SHStates::getInspectedSGObject",
    result = m_shStateVal.callMethod("SGCanvasOperator", "getInspectedObjectPropertyGenerator", 0, 0);
    if(result.isNullObject())
      result = FabricCore::RTVal();
 );
  return result;
}

SHGLScene* SHStates::getActiveScene() {
  return m_activeSHGLScene;
}

void SHStates::onStateChanged() {
  FABRIC_TRY("SHStates::onStateChanged",
    if(m_shStateVal.callMethod("Boolean", "sceneHierarchyChanged", 0, 0).getBoolean())
      emit sceneHierarchyChanged();
    if(m_shStateVal.callMethod("Boolean", "sceneChanged", 0, 0).getBoolean())
      emit sceneChanged();
    if(m_shStateVal.callMethod("Boolean", "selectionChanged", 0, 0).getBoolean())
      emit selectionChanged();
  );
}

void SHStates::onInspectedSGObject(FabricCore::RTVal sgObject) {
  FABRIC_TRY("SHStates::onInspectedSGObject",
    if(m_shStateVal.callMethod("Boolean", "setInspectedObject", 1, &sgObject).getBoolean())
      emit inspectedChanged();
  );
}

void SHStates::onInspectedSGObjectProperty(FabricCore::RTVal sgObjectProperty) {
  FABRIC_TRY("SHStates::onInspectedSGObjectProperty",
    if(m_shStateVal.callMethod("Boolean", "setInspectedObjectProperty", 1, &sgObjectProperty).getBoolean())
      emit inspectedChanged();
  );
}

void SHStates::onInspectedSGObjectPropertyGenerator(FabricCore::RTVal sgObjectProperty) {
  FABRIC_TRY("SHStates::onInspectedSGObjectPropertyGenerator",
    if(m_shStateVal.callMethod("Boolean", "setInspectedObjectPropertyGenerator", 1, &sgObjectProperty).getBoolean())
      emit inspectedChanged();
  );
}

void SHStates::onActiveSceneChanged(SHGLScene* scene) {
  FABRIC_TRY("SHStates::onActiveSceneChanged",
    m_activeSHGLScene = scene;
    FabricCore::RTVal sceneRTVal = scene->getSHGLScene();
    if(m_shStateVal.callMethod("Boolean", "setActiveScene", 1, &sceneRTVal).getBoolean())
      emit activeSceneChanged();
  );
}

void SHStates::onFrameChanged(int frame) {
  FABRIC_TRY("SHStates::onFrameChanged",
    RTVal arg = RTVal::ConstructSInt32(getClient(), frame);
    if(m_shStateVal.callMethod("Boolean", "setFrame", 1, &arg).getBoolean())
      emit sceneChanged();
  );
}

void SHStates::clearSelection() {
  FABRIC_TRY("SHStates::clearSelection",
    m_shStateVal.callMethod("", "clearSelection", 0, 0);
  );
  onStateChanged();
}

void SHStates::addSGObjectToSelection(FabricCore::RTVal sgObject) {
  FABRIC_TRY("SHStates::addSGObjectToSelection",
    m_shStateVal.callMethod("", "addObjectToSelection", 1, &sgObject);
  );
  onStateChanged();
}

void SHStates::removeSGObjectFromSelection(FabricCore::RTVal sgObject) {
  FABRIC_TRY("SHStates::removeSGObjectFromSelection",
    m_shStateVal.callMethod("", "removeObjectFromSelection", 1, &sgObject);
  );
  onStateChanged();
}

void SHStates::addSGObjectPropertyToSelection(FabricCore::RTVal sgObject) {
  FABRIC_TRY("SHStates::addSGObjectPropertyToSelection",
    m_shStateVal.callMethod("", "addObjectPropertyToSelection", 1, &sgObject);
  );
  onStateChanged();
}

void SHStates::removeSGObjectPropertyFromSelection(FabricCore::RTVal sgObject) {
  FABRIC_TRY("SHStates::removeSGObjectPropertyFromSelection",
    m_shStateVal.callMethod("", "removeObjectPropertyFromSelection", 1, &sgObject);
  );
  onStateChanged();
}

void SHStates::addSGObjectPropertyGeneratorToSelection(FabricCore::RTVal sgObject) {
  FABRIC_TRY("SHStates::addSGObjectPropertyGeneratorToSelection",
    m_shStateVal.callMethod("", "addObjectPropertyGeneratorToSelection", 1, &sgObject);
  );
  onStateChanged();
}

void SHStates::removeSGObjectPropertyGeneratorFromSelection(FabricCore::RTVal sgObject) {
  FABRIC_TRY("SHStates::removeSGObjectPropertyGeneratorFromSelection",
    m_shStateVal.callMethod("", "removeObjectPropertyGeneratorFromSelection", 1, &sgObject);
  );
  onStateChanged();
}
