//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include "SGObjectModelItem.h"

namespace FabricUI {
namespace SceneHub {

//////////////////////////////////////////////////////////////////////////
SGObjectModelItem::SGObjectModelItem(
  FabricCore::Client client,
  FabricCore::RTVal rtVal
  )
  : m_client( client )
  , m_rtVal( rtVal )
{
}

SGObjectModelItem::~SGObjectModelItem()
{
}


int SGObjectModelItem::getNumChildren()
{
  return 0; // todo
}

FTL::CStrRef SGObjectModelItem::getChildName( int i )
{
  return ""; // todo
}

BaseModelItem *SGObjectModelItem::createChild( FTL::CStrRef name ) /**/
{
  // return new ArgModelItem(
  //   m_dfgUICmdHandler,
  //   m_binding,
  //   name
  //   );
  return NULL; // todo
}

FTL::CStrRef SGObjectModelItem::getName()
{
  if(m_rtVal.isValid())
  {
    try
    {
      return m_rtVal.callMethod("String", "getName", 0, 0).getStringCString();
    }
    catch(FabricCore::Exception e)
    {
      printf("FabricCore::Exception: '%s'\n", e.getDesc_cstr());
    }
  }
  return FTL_STR("<Root>");
}

bool SGObjectModelItem::canRename()
{
  return true;
}

void SGObjectModelItem::rename( FTL::CStrRef newName )
{
  try
  {
    FabricCore::RTVal newNameVal = FabricCore::RTVal::ConstructString(m_client, newName.c_str());
    m_rtVal.callMethod("", "setUserName", 1, &newNameVal); // todo: does not seem to work
  }
  catch(FabricCore::Exception e)
  {
    printf("FabricCore::Exception: '%s'\n", e.getDesc_cstr());
  }
}

void SGObjectModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
  assert( false ); // todo
}

void SGObjectModelItem::setMetadataImp( 
  const char* key, 
  const char* value, 
  bool canUndo ) /**/
{
  // m_rootExec.setMetadata( key, value, canUndo ); // todo
}

QVariant SGObjectModelItem::getValue()
{
  return QVariant(); // todo
}

ItemMetadata* SGObjectModelItem::getMetadata()
{
  return NULL; // todo
}

void SGObjectModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
  // todo
  // if (commit)
  // {
  //   QByteArray asciiArr = var.toString().toAscii();
  //   m_rootExec.setTitle( asciiArr.data() );
  //   emitModelValueChanged(var);
  // }
}

} // namespace SceneHub
} // namespace FabricUI
