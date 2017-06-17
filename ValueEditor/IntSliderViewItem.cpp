//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#include "IntSlider.h"
#include "IntSliderViewItem.h"
#include "ViewConstants.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"
#include "VELineEdit.h"

#include <limits.h>
#include <QVariant>
#include <QHBoxLayout>
#include <QSlider>

#include <FabricUI/Util/UIRange.h>
#include <FTL/AutoSet.h>

using namespace FabricUI::ValueEditor;

IntSliderViewItem::IntSliderViewItem(
  QString const &name,
  QVariant const &variant,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
  , m_isSettingValue( false )
{
  m_lineEdit = new VELineEdit;
  m_lineEdit->setObjectName( "VELeft" );
  m_lineEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

  m_slider = new IntSlider;
  m_slider->setObjectName( "VERight" );
  m_slider->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

  metadataChanged();

  int value = getQVariantRTValValue<int>(variant);

  // correct the softrange
  if(value < m_slider->min())
    m_slider->setResolution(value, m_slider->max());
  if(value > m_slider->max())
    m_slider->setResolution(m_slider->min(), value);

  m_lineEdit->setValidatorInt();
  m_lineEdit->setText( QString::number( value ) );
  m_slider->setIntegerValue( value );

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_lineEdit );
  layout->addWidget( m_slider );
  
  layout->setStretchFactor(m_lineEdit, 1);
  layout->setStretchFactor(m_slider, 5);
  
  m_widget = new QWidget;
  m_widget->setObjectName( "VEIntSliderViewItem" );
  m_widget->setLayout( layout );
  
  connect(
    m_lineEdit, SIGNAL( textModified( QString ) ),
    this, SLOT( onLineEditTextModified( QString ) )
    );

  connect(
    m_slider, SIGNAL( sliderPressed() ),
    this, SLOT( onSliderPressed() )
    );
  connect(
    m_slider, SIGNAL( integerValueChanged( int ) ),
    this, SLOT( onIntegerValueChanged( int ) )
    );
  connect(
    m_slider, SIGNAL( sliderReleased() ),
    this, SLOT( onSliderReleased() )
    );
}

IntSliderViewItem::~IntSliderViewItem()
{
}

QWidget *IntSliderViewItem::getWidget()
{
  return m_widget;
}

void IntSliderViewItem::onModelValueChanged( QVariant const &v )
{
  FTL::AutoSet<bool> settingValue(m_isSettingValue, true);

  int value = getQVariantRTValValue<int>(v);

  // correct the softrange
  if(value < m_slider->min())
    m_slider->setResolution(value, m_slider->max());
  if(value > m_slider->max())
    m_slider->setResolution(m_slider->min(), value);

  m_slider->setIntegerValue( value, false );
  m_lineEdit->setText( QString::number( value ) );
}

void IntSliderViewItem::metadataChanged()
{
  // metadata should be like this:
  // uiRange: "(0, 200)"
  // uiHardRange: "(0, 500)"

  if ( !FabricUI::DecodeUIRange(
    m_metadata.getString( "uiRange" ),
    m_softMinimum,
    m_softMaximum
    ) )
  {
    m_softMinimum = 0;
    m_softMaximum = 100;
  }

  if ( !FabricUI::DecodeUIRange(
    m_metadata.getString( "uiHardRange" ),
    m_hardMinimum,
    m_hardMaximum
    ) )
  {
    m_hardMinimum = -INT_MAX;
    m_hardMaximum = +INT_MAX;
  }

  m_softMinimum = std::max( m_softMinimum, m_hardMinimum );
  m_softMaximum = std::min( m_softMaximum, m_hardMaximum );

  m_slider->setResolution( m_softMinimum, m_softMaximum );
}

void IntSliderViewItem::onLineEditTextModified( QString text )
{
  int value = std::max(
    m_hardMinimum,
    std::min(
      m_hardMaximum,
      text.toInt()
      )
    );

  // correct the softrange
  if(value < m_slider->min())
    m_slider->setResolution(value, m_slider->max());
  if(value > m_slider->max())
    m_slider->setResolution(m_slider->min(), value);

  if ( value != m_slider->integerValue() )
    m_slider->setIntegerValue( value, true /* emitSignal */  );
  else
    m_lineEdit->setText( QString::number( value ) );
}

void IntSliderViewItem::onSliderPressed()
{
  emit interactionBegin();
}

void IntSliderViewItem::onIntegerValueChanged( int value )
{
  if(m_isSettingValue)
    return;
  m_lineEdit->setText( QString::number( value ) );
  emit viewValueChanged( QVariant::fromValue<int>( value ) );
}

void IntSliderViewItem::onSliderReleased()
{
  emit interactionEnd( true );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* IntSliderViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (metaData == NULL)
    return NULL;

  if ( RTVariant::isType<int>( value ) || RTVariant::isType<unsigned>( value ) )
  {
    // We can only create the UI if we have a defined Min & Max
    if (metaData->has( "uiRange" ) || metaData->has( "uiHardRange" ))
    {
      IntSliderViewItem* item = new IntSliderViewItem( name, value, metaData );
      return item;
    }
  }
  return NULL;
}

const int IntSliderViewItem::Priority = 5;
