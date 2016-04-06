/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHDFGCanvas.h"
using namespace FabricCore;
using namespace FabricUI::SceneHub;
 

SHDFGCanvas::SHDFGCanvas(
  FabricCore::DFGBinding &binding, 
  FabricUI::DFG::DFGController *controller, 
  FabricCore::Client client) 
  : m_binding(binding)
  , m_controller(controller) 
  , m_client(client)
{ 
  connectBindingNotifier();
}

bool SHDFGCanvas::dirtyAllOutputs() {
  if(m_dfgCanvasOperator.isValid()) 
  {
    try
    {
      m_dfgCanvasOperator.callMethod("", "dirtyAllOutputs", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHDFGCanvas::dirtyAllOutputs: exception: %s\n", e.getDesc_cstr());
    }
    return true;
  }
  return false;
}

void SHDFGCanvas::connectBindingNotifier() {
  QSharedPointer<DFG::DFGBindingNotifier> notifier = m_controller->getBindingNotifier();
  QObject::connect( notifier.data(), SIGNAL( argInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) ), this, SLOT( onArgInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) ) );
  QObject::connect( notifier.data(), SIGNAL( argRemoved( unsigned, FTL::CStrRef ) ), this, SLOT( onArgRemoved( unsigned, FTL::CStrRef ) ) );
  QObject::connect( notifier.data(), SIGNAL( argTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) ), this, SLOT( onArgTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) ) );
}

void SHDFGCanvas::onArgInserted(unsigned index, FTL::CStrRef name, FTL::CStrRef typeName) {
  if(!m_dfgCanvasOperator.isValid()) return;
  try 
  {
    FabricCore::RTVal args[2];
    args[0] = FabricCore::RTVal::Construct( m_client, "SGContext", 0, 0 );
    args[1] = FabricCore::RTVal::ConstructString( m_client, name.c_str() );
    m_dfgCanvasOperator.callMethod("", "addPropertyForArgument", 2, args);
  }
  catch(Exception e)
  {
    printf("SHDFGCanvas::onArgInserted: exception: %s\n", e.getDesc_cstr());
  }
}

void SHDFGCanvas::onArgRemoved(unsigned index, FTL::CStrRef name) {
  if(!m_dfgCanvasOperator.isValid()) return;
  try 
  {
    FabricCore::RTVal args[2];
    args[0] = FabricCore::RTVal::Construct( m_client, "SGContext", 0, 0 );
    args[1] = FabricCore::RTVal::ConstructString( m_client, name.c_str() );
    m_dfgCanvasOperator.callMethod("", "removePropertyForArgument", 2, args);
  }
  catch(Exception e)
  {
    printf("SHDFGCanvas::getAssetLibraryRoot: exception: %s\n", e.getDesc_cstr());
  }
}

void SHDFGCanvas::onArgTypeChanged(unsigned index, FTL::CStrRef name, FTL::CStrRef newTypeName) {
  if(!m_dfgCanvasOperator.isValid())  return;
  try 
  {
    FabricCore::RTVal args[2];
    args[0] = FabricCore::RTVal::Construct( m_client, "SGContext", 0, 0 );
    args[1] = FabricCore::RTVal::ConstructString( m_client, name.c_str() );

    m_dfgCanvasOperator.callMethod("", "removePropertyForArgument", 2, args);
    m_dfgCanvasOperator.callMethod("", "addPropertyForArgument", 2, args);
  }
  catch(Exception e)
  {
    printf("SHDFGCanvas::onArgTypeChanged: exception: %s\n", e.getDesc_cstr());
  }
}

void SHDFGCanvas::onTreeItemSelected(FabricUI::SceneHub::SHTreeItem *item) {

  m_dfgCanvasSgObject = FabricCore::RTVal();

  // try to see if this object has a DFGBinding behind it
  try 
  {
    FabricCore::RTVal sgObj = item->getSGObject();
    if( !sgObj.isValid() ) return;

    FabricCore::RTVal sgContext = FabricCore::RTVal::Construct( m_client, "SGContext", 0, 0 );
    FabricCore::RTVal canvasTarget = sgObj.callMethod( "SGCanvasTarget", "getWrapper", 1, &sgContext );
    if( !canvasTarget.isNullObject() ) 
    {
      m_dfgCanvasOperator = canvasTarget.callMethod( "Ref<SGCanvasOperator>", "getSGCanvasOperator", 0, 0 );
      if( !m_dfgCanvasOperator.isNullObject() ) 
      {
        FabricCore::RTVal dfgBindingVal = m_dfgCanvasOperator.callMethod( "DFGBinding", "getDFGBinding", 0, 0 );
        FabricCore::DFGBinding binding = dfgBindingVal.getDFGBinding();
        FabricCore::DFGExec exec = binding.getExec();

        m_controller->setBindingExec( binding, "", exec );
        m_dfgCanvasSgObject = sgObj;
      } 
      else 
      {
        // return to the standard binding
        m_dfgCanvasOperator = FabricCore::RTVal();
        FabricCore::DFGExec exec =m_controller->getBinding().getExec();
        m_controller->setBindingExec(m_binding, "", exec );
      }
    } 
    else 
    {
      // return to the standard binding
      m_dfgCanvasOperator = FabricCore::RTVal();
      FabricCore::DFGExec exec =m_controller->getBinding().getExec();
      m_controller->setBindingExec(m_binding, "", exec );
    }
  } 
  catch( FabricCore::Exception e ) {
    printf("SHDFGCanvas::onTreeItemSelected: exception: %s\n", e.getDesc_cstr());
  }

  connectBindingNotifier();
}
