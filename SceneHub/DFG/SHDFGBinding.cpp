/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHDFGBinding.h"
using namespace FabricCore;
using namespace FabricUI;
using namespace SceneHub;
 

SHDFGBinding::SHDFGBinding(
  DFGBinding &binding, 
  DFG::DFGController *controller,
  SHStates *shStates) 
  : m_binding(binding)
  , m_controller(controller) 
  , m_shStates(shStates)
{ 
  connectBindingNotifier();
  m_computeContextVal = RTVal::Create( controller->getClient(), "ComputeContextRTValWrapper", 0, 0 );
}

QList<bool> SHDFGBinding::setDirty() {
  QList<bool> dirtyList;
  // accepted
  dirtyList.append(m_dfgCanvasOperator.isValid());
  // refresh
  dirtyList.append(false);
  if( dirtyList[0] )
  {
    try
    {
      //Ignore: this is from runtime KL Bindings computation (we are already redrawing)
      if( ! m_computeContextVal.callMethod( "Boolean", "hasExecutingKLDFGBinding", 0, 0 ).getBoolean() )
      {
        dirtyList[1] = true;
        m_dfgCanvasOperator.callMethod("", "setDirty", 0, 0);
      }
    }
    catch(Exception e)
    {
      printf("SHDFGBinding::setDirty: exception: %s\n", e.getDesc_cstr());
    }
  }
  return dirtyList;
}

void SHDFGBinding::connectBindingNotifier() {
  QSharedPointer<DFG::DFGBindingNotifier> notifier = m_controller->getBindingNotifier();
  QObject::connect( notifier.data(), SIGNAL( argInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) ), this, SLOT( onArgInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) ) );
  QObject::connect( notifier.data(), SIGNAL( argRemoved( unsigned, FTL::CStrRef ) ), this, SLOT( onArgRemoved( unsigned, FTL::CStrRef ) ) );
  QObject::connect( notifier.data(), SIGNAL( argTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) ), this, SLOT( onArgTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) ) );
}

void SHDFGBinding::onArgInserted(unsigned index, FTL::CStrRef name, FTL::CStrRef typeName) {
  if(!m_dfgCanvasOperator.isValid()) return;
  try 
  {
    m_dfgCanvasOperator.callMethod( "", "updatePropertySetFromBinding", 0, 0 );
  }
  catch(Exception e)
  {
    printf("SHDFGBinding::onArgInserted: exception: %s\n", e.getDesc_cstr());
  }
  emit sceneChanged();
}

void SHDFGBinding::onArgRemoved(unsigned index, FTL::CStrRef name) {
  if(!m_dfgCanvasOperator.isValid()) return;
  try 
  {
    m_dfgCanvasOperator.callMethod( "", "updatePropertySetFromBinding", 0, 0 );
  }
  catch(Exception e)
  {
    printf("SHDFGBinding::getAssetLibraryRoot: exception: %s\n", e.getDesc_cstr());
  }
  emit sceneChanged();
}

void SHDFGBinding::onArgTypeChanged(unsigned index, FTL::CStrRef name, FTL::CStrRef newTypeName) {
  if(!m_dfgCanvasOperator.isValid())  return;
  try 
  {
    m_dfgCanvasOperator.callMethod( "", "updatePropertySetFromBinding", 0, 0 );
  }
  catch(Exception e)
  {
    printf("SHDFGBinding::onArgTypeChanged: exception: %s\n", e.getDesc_cstr());
  }
  emit sceneChanged();
}

void SHDFGBinding::setCanvasOperator( FabricCore::RTVal &canvasOperator ) {
  try {
    m_dfgCanvasOperator = canvasOperator;
    if( m_dfgCanvasOperator.isValid() ) {
      RTVal dfgBindingVal = m_dfgCanvasOperator.callMethod( "DFGBinding", "getDFGBinding", 0, 0 );
      DFGBinding binding = dfgBindingVal.getDFGBinding();
      DFGExec exec = binding.getExec();
      m_controller->setBindingExec( binding, "", exec );
    } else {
      // return to the standard binding
      DFGExec exec = m_binding.getExec();
      m_controller->setBindingExec( m_binding, "", exec );
    }
  }
  catch( Exception e ) {
    printf( "SceneHubWindow::setCanvasOperator: exception: %s\n", e.getDesc_cstr() );
  }
  connectBindingNotifier();
}

FabricCore::RTVal SHDFGBinding::getCanvasOperatorParameterObject() {
  try {
    if( m_dfgCanvasOperator.isValid() ) {
      RTVal parameterObject = m_dfgCanvasOperator.callMethod( "SGObject", "getParameterContainer", 0, 0 );
      if( parameterObject.callMethod( "Boolean", "isValid", 0, 0 ).getBoolean() )
        return parameterObject;
    }
  }
  catch( Exception e ) {
    printf( "SceneHubWindow::getCanvasOperatorParameterObject: exception: %s\n", e.getDesc_cstr() );
  }
  return RTVal();
}

void SHDFGBinding::onInspectChanged() {
  FabricCore::RTVal canvasOperator = m_shStates->getInspectedSGCanvasOperator();
  setCanvasOperator( canvasOperator );
}

void SHDFGBinding::setMainBinding( FabricCore::DFGBinding &binding ) {
  m_dfgCanvasOperator = RTVal();
  m_binding = binding;
}
