//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "BaseModelItem.h"
#include <FabricCore.h>

using namespace FabricUI::ValueEditor;

BaseModelItem::BaseModelItem()
  : m_interactionBracketCount( 0 )
  , m_modelValueChangedBracketCount( 0 )
  , m_settingMetadata( false ) 
{
}

BaseModelItem::~BaseModelItem()
{
}

int BaseModelItem::getNumChildren()
{
  return 0;
}

BaseModelItem* BaseModelItem::getChild( FTL::StrRef childName, bool doCreate )
{
  int res = getChildIndex( childName );
  if (res >= 0)
    return getChild( res, doCreate );
  return NULL;
}

BaseModelItem* BaseModelItem::getChild( int index, bool doCreate )
{
  return NULL;
}

FabricUI::ValueEditor::BaseModelItem *
BaseModelItem::getDescendant( FTL::StrRef descendantPath )
{
  FTL::StrRef::Split split = descendantPath.split('.');

  FabricUI::ValueEditor::BaseModelItem *item = this;
  while ( !split.first.empty() && !!item )
  {
    item = item->getChild( split.first, false );
    split = split.second.split('.');
  }
  return item;
}

FTL::CStrRef BaseModelItem::getChildName( int i )
{
  BaseModelItem* pChild = getChild( i );
  if ( pChild != NULL )
    return pChild->getName();
  return FTL::CStrRef();
}

int BaseModelItem::getChildIndex( FTL::StrRef childName )
{
  int numChildren = getNumChildren();
  for ( int i = 0; i < numChildren; i++ )
  {
    if (getChildName( i ) == childName)
      return i;
  }
  return -1;
}

ItemMetadata* BaseModelItem::getMetadata()
{
  return NULL;
}

void BaseModelItem::setMetadata( const char* key, const char* val, bool canUndo )
{
  m_settingMetadata = true;
  setMetadataImp( key, val, canUndo );
  m_settingMetadata = false;
}

bool BaseModelItem::isSettingMetadata()
{
  return m_settingMetadata;
}

void BaseModelItem::onInteractionBegin()
{
  if ( m_interactionBracketCount++ == 0 )
    m_valueAtInteractionBegin = getValue();
}

void BaseModelItem::onViewValueChanged(
  QVariant value
  )
{
  if ( m_modelValueChangedBracketCount == 0 )
  {
    setValue(
      value,
      m_interactionBracketCount == 0,
      m_valueAtInteractionBegin
      );
  }
}

void BaseModelItem::onInteractionEnd( bool accept )
{
  if ( --m_interactionBracketCount == 0 )
  {
    if ( accept )
    {
      setValue(
        getValue(),
        true, // commit
        m_valueAtInteractionBegin
        );
    }
    else
    {
      setValue(
        m_valueAtInteractionBegin,
        false,
        m_valueAtInteractionBegin
        );
    }

    m_valueAtInteractionBegin = QVariant();
  }
}
