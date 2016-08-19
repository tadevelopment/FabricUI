//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DictionaryViewItem.h"
#include "ViewItemFactory.h"
#include "QVariantRTVal.h"
#include <QtGui/QWidget>
#include <QtGui/QBoxLayout>
#include <QtGui/QLabel>

using namespace FabricUI::ValueEditor;

DictionaryViewItem::DictionaryViewItem(
  QString name,
  const FabricCore::RTVal& value,
  ItemMetadata* metadata
)
  : BaseComplexViewItem( name, metadata )
  , m_val( value )
  , m_widget( NULL )
{
  m_widget = new QWidget();
  
  // Display the type of the dictionary
  //QHBoxLayout *layout = new QHBoxLayout( m_widget );
  //layout->addWidget( new QLabel( m_val.getTypeName().getStringCString() ) );
}

DictionaryViewItem::~DictionaryViewItem()
{
  
}

void DictionaryViewItem::doAppendChildViewItems( QList<BaseViewItem *>& items )
{
  try
  {
    ViewItemFactory* factory = ViewItemFactory::GetInstance();
    FabricCore::RTVal keys = m_val.getDictKeys();
    for( unsigned int i = 0; i < keys.getArraySize(); i++ )
    {
      FabricCore::RTVal key = keys.getArrayElement( i );
      FabricCore::RTVal childVal = m_val.getDictElement( key );
      BaseViewItem* childItem =
        factory->createViewItem(
          key.getStringCString(),
          toVariant( childVal ),
          &m_metadata
        );

      if( childItem != NULL )
      {
        connectChild( i, childItem );
        items.push_back( childItem );
      }
    }

  }
  catch (FabricCore::Exception e)
  {
    printf( "DictionaryViewItem::doAppendChildViewItems : %s\n", e.getDesc_cstr() );
  }
}

void DictionaryViewItem::onChildViewValueChanged( int index, QVariant value )
{
  try
  {
    if( m_val.isValid() && m_val.isDict() )
    {
      FabricCore::RTVal key = m_val.getDictKeys().getArrayElement( index );
      FabricCore::RTVal oldChildVal = m_val.getDictElement( key );
      RTVariant::toRTVal( value, oldChildVal );
      m_val.setDictElement( key, oldChildVal );
    }
    emit viewValueChanged( toVariant( m_val ) );
  }
  catch (FabricCore::Exception e)
  {
    printf( "DictionaryViewItem::onChildViewValueChanged : %s\n", e.getDesc_cstr() );
  }
}

QWidget * DictionaryViewItem::getWidget()
{
  return m_widget;
}

void DictionaryViewItem::onModelValueChanged( QVariant const &value )
{
  try
  {
    RTVariant::toRTVal( value, m_val );

    FabricCore::RTVal keys = m_val.getDictKeys();
    for( unsigned int i = 0; i < keys.getArraySize(); i++ )
    {
      FabricCore::RTVal key = keys.getArrayElement( i );
      FabricCore::RTVal childVal = m_val.getDictElement( key );
      routeModelValueChanged( i, toVariant( childVal ) );
    }
  }
  catch (FabricCore::Exception e)
  {
    printf( "DictionaryViewItem::onModelValueChanged : %s\n", e.getDesc_cstr() );
  }
}

BaseViewItem* DictionaryViewItem::CreateItem(
  QString const &name, 
  QVariant const &value,
  ItemMetadata* metadata
)
{
  if(value.type() != QVariant::UserType)
    return NULL;
  if (value.userType() != qMetaTypeId<FabricCore::RTVal>())
    return NULL;

  FabricCore::RTVal rtVal = value.value<FabricCore::RTVal>();
  if( rtVal.isValid() && rtVal.isDict() && rtVal.dictKeyHasType( "String" ) )
  {
    DictionaryViewItem* pViewItem = new DictionaryViewItem( QString(name), rtVal, metadata );
    return pViewItem;
  }
  return NULL;
};

const int DictionaryViewItem::Priority = 5;
