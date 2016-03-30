//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <assert.h>
#include "SGObjectModelItem.h"
#include "SGObjectPropertyModelItem.h"

namespace FabricUI {
namespace SceneHub {

//////////////////////////////////////////////////////////////////////////
SGObjectModelItem::SGObjectModelItem(
  SHCmdViewWidget * cmdViewWidget,
  FabricCore::Client client,
  FabricCore::RTVal rtVal
  )
  : m_cmdViewWidget( cmdViewWidget )
  , m_client( client )
  , m_rtVal( rtVal )
{
  m_lastStructureVersionRtVal = FabricCore::RTVal::ConstructUInt32( m_client, 0 );
  m_lastValuesVersionRtVal = FabricCore::RTVal::ConstructUInt32( m_client, 0 );
  m_isValidRtVal = FabricCore::RTVal::ConstructBoolean( m_client, false );
  m_structureChangedRtVal = FabricCore::RTVal::ConstructBoolean( m_client, false );
  m_valueChangedRtVal = FabricCore::RTVal::ConstructBoolean( m_client, false );

  // Cache initial versions
  bool isValid, structureChanged;
  updateFromScene( rtVal, isValid, structureChanged );
}

SGObjectModelItem::~SGObjectModelItem()
{
}

int SGObjectModelItem::getNumChildren()
{
  ensurePropertiesRTVal();
  if(m_propertiesRtVal.isValid())
  {
    try
    {
      return m_propertiesRtVal.getArraySize();
    }
    catch(FabricCore::Exception e)
    {
      printf("SGObjectModelItem::getNumChildren, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
    }
  }
  return 0;
}

FTL::CStrRef SGObjectModelItem::getChildName( int i )
{
  ensurePropertiesRTVal();

  std::map<std::string, unsigned int>::iterator it = m_propertyNameMap.begin();
  for(int offset=0;it != m_propertyNameMap.end(); offset++,it++)
  {
    if(offset == i)
    {
      return it->first.c_str();
    }
  }

  return "";
}

BaseModelItem *SGObjectModelItem::createChild( FTL::CStrRef name ) /**/
{
  ensurePropertiesRTVal();
  if(m_propertiesRtVal.isValid())
  {
    std::map<std::string, unsigned int>::iterator it = m_propertyNameMap.find(name);
    if(it == m_propertyNameMap.end())
      return NULL;

    try
    {
      if(it->second >= m_propertiesRtVal.getArraySize())
        return NULL;

      FabricCore::RTVal propRtVal = m_propertiesRtVal.getArrayElement(it->second);
      BaseModelItem * child = pushChild(new SGObjectPropertyModelItem(m_cmdViewWidget, m_client, propRtVal));
      emit propertyItemInserted(child);
      return child;
    }
    catch(FabricCore::Exception e)
    {
      printf("SGObjectModelItem::createChild, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
    }
  }
  return NULL;
}

FTL::CStrRef SGObjectModelItem::getName()
{
  if(m_rtVal.isValid())
  {
    try
    {
      m_name = m_rtVal.callMethod("String", "getName", 0, 0).getStringCString();
      return m_name;;
    }
    catch(FabricCore::Exception e)
    {
      printf("SGObjectModelItem::createChild, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
    }
  }
  return FTL_STR("<Root>");
}

bool SGObjectModelItem::canRename()
{
  return false;
}

void SGObjectModelItem::rename( FTL::CStrRef newName )
{
}

void SGObjectModelItem::onRenamed(
  FTL::CStrRef oldName,
  FTL::CStrRef newName
  )
{
}

QVariant SGObjectModelItem::getValue()
{
  return QVariant();
}

ItemMetadata* SGObjectModelItem::getMetadata()
{
  return NULL;
}

void SGObjectModelItem::setMetadataImp( 
  const char* key, 
  const char* value, 
  bool canUndo )
{
}

void SGObjectModelItem::setValue(
  QVariant var,
  bool commit,
  QVariant valueAtInteractionBegin
  )
{
}

void SGObjectModelItem::onStructureChanged()
{
  m_propertyNameMap.clear();
  m_propertiesRtVal = FabricCore::RTVal();
}

void SGObjectModelItem::ensurePropertiesRTVal()
{
  if(m_propertiesRtVal.isValid())
    return;

  if(!m_rtVal.isValid())
    return;

  m_propertyNameMap.clear();
  m_propertiesRtVal = FabricCore::RTVal();

  try
  {
    m_propertiesRtVal =  m_rtVal.callMethod("SGObjectProperty[]", "getEditablePropertyArray", 0, 0);
    for(unsigned int i=0;i<m_propertiesRtVal.getArraySize();i++)
    {
      FabricCore::RTVal nameVal = m_propertiesRtVal.getArrayElement(i).callMethod("String", "getName", 0, 0);
      m_propertyNameMap.insert(std::pair<std::string, unsigned int>(nameVal.getStringCString(), i));
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("SGObjectModelItem::ensurePropertiesRTVal, FabricCore::Exception: '%s'\n", e.getDesc_cstr());
  }
}

void SGObjectModelItem::updateFromScene( const FabricCore::RTVal& newSGObject, bool& isValid, bool& structureChanged ) {
  try {
    FabricCore::RTVal args[6];
    args[0] = newSGObject;
    args[1] = m_lastStructureVersionRtVal;
    args[2] = m_lastValuesVersionRtVal;
    args[3] = m_isValidRtVal;
    args[4] = m_structureChangedRtVal;
    args[5] = m_valueChangedRtVal;

    m_rtVal.callMethod( "", "synchronize", 6, args );

    isValid = m_isValidRtVal.getBoolean();
    structureChanged = m_structureChangedRtVal.getBoolean();
    if( isValid && !structureChanged && m_valueChangedRtVal.getBoolean() ) {
      // Update individual property values
      int count = getNumChildren();
      for( int i = 0; i < count; ++i ) {
        SGObjectPropertyModelItem * objectItem = dynamic_cast< SGObjectPropertyModelItem * >( getChild( i, false ) );
        if( objectItem )
          objectItem->updateFromScene();
      }
    }
  }
  catch( FabricCore::Exception e ) {
    printf( "SGObjectModelItem::updateFromScene, FabricCore::Exception: '%s'\n", e.getDesc_cstr() );
  }
}

} // namespace SceneHub
} // namespace FabricUI
