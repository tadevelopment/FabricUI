/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHDFGBinding.h"
using namespace FabricCore;
using namespace FabricUI::SceneHub;
 

SHDFGBinding::SHDFGBinding(
  FabricCore::DFGBinding &binding, 
  FabricUI::DFG::DFGController *controller, 
  FabricCore::Client client) 
  : m_binding(binding)
  , m_controller(controller) 
  , m_client(client)
{ 
  connectBindingNotifier();
}

bool SHDFGBinding::dirtyAllOutputs() {
  if(m_dfgCanvasOperator.isValid()) 
  {
    try
    {
      m_dfgCanvasOperator.callMethod("", "dirtyAllOutputs", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHDFGBinding::dirtyAllOutputs: exception: %s\n", e.getDesc_cstr());
    }
    return true;
  }
  return false;
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
}

void SHDFGBinding::onTreeItemSelected(FabricUI::SceneHub::SHTreeItem *item) {

  m_dfgCanvasSgObject = FabricCore::RTVal();

  // try to see if this object has a DFGBinding behind it
  try 
  {
    m_dfgCanvasOperator = item->getSGCanvasOperator();
    if( m_dfgCanvasOperator.isValid() ) 
    {
      FabricCore::RTVal dfgBindingVal = m_dfgCanvasOperator.callMethod( "DFGBinding", "getDFGBinding", 0, 0 );
      FabricCore::DFGBinding binding = dfgBindingVal.getDFGBinding();
      FabricCore::DFGExec exec = binding.getExec();
      m_controller->setBindingExec( binding, "", exec );
      m_dfgCanvasSgObject = item->getSGObject();
    } 
    else 
    {
      // return to the standard binding
      m_dfgCanvasOperator = FabricCore::RTVal();
      FabricCore::DFGExec exec = m_binding.getExec();
      m_controller->setBindingExec( m_binding, "", exec );
    }
  } 
  catch( FabricCore::Exception e ) 
  {
    printf( "SceneHubWindow::treeItemSelected: exception: %s\n", e.getDesc_cstr() );
  }

  connectBindingNotifier();
}
