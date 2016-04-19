/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHStates.h"
#include <FabricUI/Util/macros.h>

using namespace FabricCore;
using namespace FabricUI::SceneHub;

SHStates::SHStates( Client client ) : m_client( client ) {
printf("SHStates::SHStates");
  FABRIC_TRY( "SHStates::SHStates",
    m_shStateVal = RTVal::Create( client, "SHStates", 0, 0 );
  );
}

FabricCore::Client SHStates::getClient() {
  return m_client; 
}

FabricCore::RTVal SHStates::getSelectedObjects() {
printf( "SHStates::getSelectedObjects" );
  RTVal selectedObject;
  FABRIC_TRY_IGNORE( "SHStates::getSelectedObjects",
    selectedObject = m_shStateVal.callMethod( "SGObject[]", "getSelectedObjects", 0, 0 );
  );
  return selectedObject;
}

bool SHStates::isInspectingSGObject() {
printf( "SHStates::isInspectingSGObject" );
  bool result = false;
  FABRIC_TRY_IGNORE( "SHStates::isInspectingSGObject",
    result = m_shStateVal.callMethod( "SGObject[]", "isInspectingObject", 0, 0 ).getBoolean();
  );
  return result;
}

bool SHStates::isInspectingSGObjectProperty() {
  printf( "SHStates::isInspectingSGObjectProperty" );
  bool result = false;
  FABRIC_TRY_IGNORE( "SHStates::isInspectingSGObjectProperty",
    result = m_shStateVal.callMethod( "SGObject[]", "isInspectingObject", 0, 0 ).getBoolean();
  );
  return result;
}

bool SHStates::isInspectingSGCanvasOperator() {
  printf( "SHStates::isInspectingSGCanvasOperator" );
  bool result = false;
  FABRIC_TRY_IGNORE( "SHStates::isInspectingSGCanvasOperator",
    result = m_shStateVal.callMethod( "SGObject[]", "isInspectingSGCanvasOperator", 0, 0 ).getBoolean();
  );
  return result;
}

FabricCore::RTVal SHStates::getInspectedSGObject() {
  printf( "SHStates::getInspectedSGObject" );
  FabricCore::RTVal result;
  FABRIC_TRY_IGNORE( "SHStates::getInspectedSGObject",
    FabricCore::RTVal validRTVal = FabricCore::RTVal::ConstructBoolean( m_client, false );
    result = m_shStateVal.callMethod( "SGObject", "getInspectedObject", 1, &validRTVal );
    if( !validRTVal.getBoolean() )
      result = FabricCore::RTVal();
  );
  return result;
}

FabricCore::RTVal SHStates::getInspectedSGObjectProperty() {
  printf( "SHStates::getInspectedSGObjectProperty" );
  FabricCore::RTVal result;
  FABRIC_TRY_IGNORE( "SHStates::getInspectedSGObjectProperty",
    FabricCore::RTVal validRTVal = FabricCore::RTVal::ConstructBoolean( m_client, false );
    result = m_shStateVal.callMethod( "SGObject", "getInspectedSGObjectProperty", 1, &validRTVal );
    if( !validRTVal.getBoolean() )
      result = FabricCore::RTVal();
  );
  return result;
}

FabricCore::RTVal SHStates::getInspectedSGCanvasOperator() {
  printf( "SHStates::getInspectedSGCanvasOperator" );
  FabricCore::RTVal result;
  FABRIC_TRY_IGNORE( "SHStates::getInspectedSGObject",
    result = m_shStateVal.callMethod( "SGCanvasOperator", "getInspectedObjectPropertyGenerator", 0, 0 );
    if( result.isNullObject() )
      result = FabricCore::RTVal();
  );
  return result;
}

void SHStates::onStateChanged() {
  printf( "SHStates::onStateChanged" );
  FABRIC_TRY( "SHStates::onStateChanged",
    if( m_shStateVal.callMethod( "Boolean", "sceneHierarchyChanged", 0, 0 ).getBoolean() ) {
      printf( "SHStates::onStateChanged emit sceneHierarchyChanged" );
      emit sceneHierarchyChanged();
    }
    if( m_shStateVal.callMethod( "Boolean", "sceneChanged", 0, 0 ).getBoolean() ) {
      printf( "SHStates::onStateChanged emit sceneChanged" );
      emit sceneChanged();
    }
    if( m_shStateVal.callMethod( "Boolean", "selectionChanged", 0, 0 ).getBoolean() ) {
      printf( "SHStates::onStateChanged emit selectionChanged" );
      emit selectionChanged();
    }
  );
}

void SHStates::onInspectedSGObject( FabricCore::RTVal sgObject ) {
  printf( "SHStates::onInspectedSGObject" );
  FABRIC_TRY( "SHStates::onInspectedSGObject",
    if( m_shStateVal.callMethod( "Boolean", "setInspectedObject", 1, &sgObject ).getBoolean() )
      emit inspectedChanged();
  );
}

void SHStates::onInspectedSGObjectProperty( FabricCore::RTVal sgObjectProperty ) {
  printf( "SHStates::onInspectedSGObjectProperty" );
  FABRIC_TRY( "SHStates::onInspectedSGObjectProperty",
    if( m_shStateVal.callMethod( "Boolean", "setInspectedObjectProperty", 1, &sgObjectProperty ).getBoolean() )
      emit inspectedChanged();
  );
}

void SHStates::onInspectedSGObjectPropertyGenerator( FabricCore::RTVal sgObjectProperty ) {
  printf( "SHStates::onInspectedSGObjectPropertyGenerator" );
  FABRIC_TRY( "SHStates::onInspectedSGObjectPropertyGenerator",
    if( m_shStateVal.callMethod( "Boolean", "setInspectedObjectPropertyGenerator", 1, &sgObjectProperty ).getBoolean() )
      emit inspectedChanged();
  );
}
