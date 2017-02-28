//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "ComboBoxViewItem.h"
#include "ComboBox.h"
#include "QVariantRTVal.h"

#include <FTL/StrSplit.h>
#include <QHBoxLayout>
#include "QVariantRTVal.h"

using namespace FabricUI::ValueEditor;

ComboBoxViewItem::ComboBoxViewItem( QString const &name, QVariant const &v, ItemMetadata* metadata, bool isString, bool isRotationOrder)
  : BaseViewItem(name, metadata)
  , m_comboBox(NULL)
  , m_isString(isString)
  , m_isRotationOrder(isRotationOrder)
{
  m_comboBox = new ComboBox;

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_comboBox );
  layout->addStretch();

  m_widget = new QWidget;
  m_widget->setObjectName( "VEComboBoxViewItem" );
  m_widget->setLayout( layout );

  metadataChanged();
  onModelValueChanged(v);

  connect( m_comboBox, SIGNAL( currentIndexChanged( int ) ),
           this, SLOT( entrySelected( int ) ) );
}

ComboBoxViewItem::~ComboBoxViewItem()
{
}

void ComboBoxViewItem::metadataChanged()
{
  if ( m_isRotationOrder )
  {
    m_comboBox->clear();
    m_comboBox->addItem( "zyx" );
    m_comboBox->addItem( "xzy" );
    m_comboBox->addItem( "yxz" );
    m_comboBox->addItem( "yzx" );
    m_comboBox->addItem( "xyz" );
    m_comboBox->addItem( "zxy" );
  }
  else
  {
    const char* str = m_metadata.getString( "uiCombo" );
    if (str == NULL)
      return;

    std::string uiComboStr = str;
    if (uiComboStr.size() > 0)
    {
      if (uiComboStr[0] == '(')
        uiComboStr = uiComboStr.substr( 1 );
      if (uiComboStr[uiComboStr.size() - 1] == ')')
        uiComboStr = uiComboStr.substr( 0, uiComboStr.size() - 1 );

      QStringList parts = QString( uiComboStr.c_str() ).split( ',' );
      // Push options to UI combobox
      m_comboBox->clear();
      for (int i = 0; i < parts.size(); i++)
      {
        QString itemStr = parts[i].trimmed();
        if (itemStr.startsWith( "\"" ))
          itemStr.remove( 0, 1 );
        if (itemStr.endsWith( "\"" ) )
          itemStr.chop( 1 );

        m_comboBox->addItem( itemStr );
      }
    }
  }
}

QWidget *ComboBoxViewItem::getWidget()
{
  return m_widget;
}

void ComboBoxViewItem::onModelValueChanged( QVariant const &v )
{
  if( m_isString )
  {
    int index = m_comboBox->findText( getQVariantRTValValue<QString>(v) );
    m_comboBox->setCurrentIndex( index );
  }
  else
  {
    m_comboBox->setCurrentIndex( getQVariantRTValValue<int>(v) );
  }
}

void ComboBoxViewItem::entrySelected(int index)
{
  if( m_isString )
  {
    emit viewValueChanged(
      QVariant::fromValue<QString>( m_comboBox->itemText( index ) )
      );
  }
  else
  {
    emit viewValueChanged(
      QVariant::fromValue<int>( index )
      );
  }
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* ComboBoxViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  FabricCore::RTVal rtVal = value.value<FabricCore::RTVal>();
  bool isString = RTVariant::canConvert( value, QVariant::String );

  if ( !rtVal.isValid() )
    return 0;
  else
  {
    if ( rtVal.hasType( "RotationOrder" ) )
    {
      return new ComboBoxViewItem( name, value, metaData, isString , true);
    }
    else
    { 
      if ( metaData != NULL &&
           metaData->has("uiCombo") )
      {
        if ( RTVariant::canConvert( value, QVariant::Int ) ||
             RTVariant::canConvert( value, QVariant::UInt ) || 
             isString )
        {
          return new ComboBoxViewItem( name, value, metaData, isString );
        }
      }
      return 0;
    }
  }
}

const int ComboBoxViewItem::Priority = 5;
